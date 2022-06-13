import amqplib from 'amqplib';

import config from '../config.json';

let connection: amqplib.Connection;
let channel: amqplib.Channel;

let buffer: Buffer[] = [];
let lastMessageTime: Date;

const k = 3;
const expiredMessageThresholdSeconds = 2;

async function run() {
    await initMessaging();

    await acceptMessages();

    checkExpiredMessages();

    console.log('Broker-proxy: ready');
}

async function initMessaging() {
    connection = await amqplib.connect('amqp://localhost');
    channel = await connection.createChannel();

    // exchange for broadcast

    await channel.assertExchange(config.broadcastQueueReal, 'fanout', { durable: true });

    // queue for accept requests/forcasts

    await channel.assertQueue(config.broadcastQueue);
}

async function acceptMessages() {
    await channel.consume(config.broadcastQueue, async (msg) => {
        const messageRaw: Buffer = msg?.content as Buffer;

        lastMessageTime = new Date();

        if (msg) {
            channel.ack(msg);
        }

        console.log(JSON.parse(messageRaw.toString()));

        buffer.push(messageRaw);

        if (buffer.length >= k) {
            sendReversed();
        }
    });
}

function checkExpiredMessages() {
    setInterval(() => {
        if (!lastMessageTime) {
            return;
        }

        if ((new Date().getTime() - lastMessageTime.getTime()) / 1000 > expiredMessageThresholdSeconds && buffer.length) {
            sendReversed();
        }
    }, expiredMessageThresholdSeconds);
}

function sendReversed() {
    const reversed = buffer.slice();
    reversed.reverse();

    console.log('---------');
    console.log('Sending');

    for (const item of reversed) {
        console.log(JSON.parse(item.toString()));
        channel.publish(config.broadcastQueueReal, '', item);
    }

    console.log('---------');

    buffer = [];
}

run();
