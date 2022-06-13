import amqplib from 'amqplib';

import config from '../config.json';

const totalMessages = (config.totalSailors - 1) + config.totalSailors * (config.totalSailors - 1) - (config.totalSailors - 1);

const clock = new Array(config.totalSailors).fill(0);

let id: number;

let connection: amqplib.Connection;
let channel: amqplib.Channel;

let messagesCount = 0;

const unorderedMessages: any = [];
const bufferedMessages: any[] = [];
const orderedMessages: any = [];

let broadcastRealQueue: amqplib.Replies.AssertQueue;

async function run() {
    getArgs();

    console.log(`Sailor #${id}: ready`);

    await initMessaging();

    await listenForcastRequests();

    scheduleForcastRequest();
}

function getArgs() {
    id = Number(process.argv[2]);
}

async function initMessaging() {
    connection = await amqplib.connect('amqp://localhost');
    channel = await connection.createChannel();

    // queue for subscription to broadcast

    await channel.assertExchange(config.broadcastQueueReal, 'fanout');

    broadcastRealQueue = await channel.assertQueue('', { exclusive: true, durable: true });
    await channel.bindQueue(broadcastRealQueue.queue, config.broadcastQueueReal, '');

    // queue for publishing

    await channel.assertQueue(config.broadcastQueue);
}

async function listenForcastRequests() {
    await channel.consume(broadcastRealQueue.queue, async (msg) => {
        const messageRaw: any = msg?.content.toString();
        const message = JSON.parse(messageRaw);

        const isRequest = !message.forcastMark;

        const isMessageNeeded = isRequest && message.requestMark !== id || !isRequest && Number(message.forcastMark[0]) !== id;

        if (!isMessageNeeded) {
            if (msg) {
                channel.ack(msg);
            }

            return;
        }

        unorderedMessages.push(message);

        if (isMessageOrdered(message)) {
            processMessage(message);
            processUnorderedMessages();
        } else {
            bufferedMessages.push(message);
        }

        if (msg) {
            channel.ack(msg);
        }

        if (messagesCount === totalMessages) {
            console.log('-----------------------');
            console.log('-----------------------');
            console.log(`Sailor #${id}: FINISHED`);
            console.log('-----------------------');
            console.log(`Sailor #${id}: Unordered messages:`);
            console.log(unorderedMessages);
            console.log('-----------------------');
            console.log(`Sailor #${id}: Ordered messages:`);
            console.log(orderedMessages);
            console.log('-----------------------');
            console.log('-----------------------');
        }
    });
}

function processMessage(message: any) {
    orderedMessages.push(message);

    const isRequest = !message.forcastMark;

    console.log(`Sailor #${id}: incoming clock - ${message.clock}`);

    syncClock(message.clock);
    messagesCount++;

    if (isRequest && message.requestMark !== id) {
        console.log(`Sailor #${id}: request #${message.requestMark}`);

        updateProcessClock();
        const forcastMessage = {
            requestMark: message.requestMark,
            forcastMark: String(id) + String(message.requestMark),
            clock
        };

        channel.sendToQueue(config.broadcastQueue, Buffer.from(JSON.stringify(forcastMessage)));
    } else if (!isRequest && Number(message.forcastMark[0]) !== id) {
        console.log(`Sailor #${id}: forcast #${message.forcastMark}`);
    }

    console.log(`Sailor #${id}: clock - ${JSON.stringify(clock)}`);
}

function scheduleForcastRequest() {
    setTimeout(() => {
        updateProcessClock();
        const requestMessage = { requestMark: id, clock };
        channel.sendToQueue(config.broadcastQueue, Buffer.from(JSON.stringify(requestMessage)));
    }, 1000 * id);
}

function updateProcessClock() {
    clock[id - 1]++;
}

function syncClock(incomingClock: number[]) {
    for (let i = 0; i < config.totalSailors; i++) {
        clock[i] = Math.max(clock[i], incomingClock[i]);
    }
}

function isMessageOrdered(message: any) {
    const isRequest = !message.forcastMark;

    const senderId = isRequest ? message.requestMark : Number(message.forcastMark[0]);

    const senderCondition = (clock[senderId - 1] === message.clock[senderId - 1] - 1);

    if (!senderCondition) {
        console.log(`Sailor #${id}: failed sender condition - from ${senderId}, my clock - ${JSON.stringify(clock)}, incoming clock - ${JSON.stringify(message.clock)}`);
        return false;
    }

    for (let i = 0; i < clock.length; i++) {
        if (i === senderId - 1) {
            continue;
        }

        if (clock[i] < message.clock[i]) {
            console.log(`Sailor #${id}: failed ${i} condition - from ${senderId}, my clock - ${JSON.stringify(clock)}, incoming clock - ${JSON.stringify(message.clock)}`);
            return false;
        }
    }

    return true;
}

function processUnorderedMessages() {
    let processed = 1;

    while (processed !== 0) {
        processed = 0;
        let processedI;
        for (let i = 0; i < bufferedMessages.length; i++) {
            if (isMessageOrdered(bufferedMessages[i])) {
                processMessage(bufferedMessages[i]);
                processed++;
                processedI = i;
                break;
            }
        }
        if (processedI !== undefined) {
            bufferedMessages.splice(processedI, 1);
        }
    }
}

run();
