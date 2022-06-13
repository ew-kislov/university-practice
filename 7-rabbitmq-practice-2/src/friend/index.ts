import crypto from 'crypto';

import amqplib from 'amqplib';

import config from './config.json';

let id: number;
let baseLatency: number;
let latencyFactor: number;

let connection: amqplib.Connection;
let channel: amqplib.Channel;
let shipQueue: amqplib.Replies.AssertQueue;

const ships: Set<string> = new Set();

async function run() {
    getArgs();

    console.log(`Friend #${id} ready - latency ${baseLatency * latencyFactor}`);

    await initMessaging();

    // await handleIdentification();

    await initSailersMessaging();

    await listenForcastRequests();
    await listenToSailors();
}

function getArgs() {
    id = Number(process.argv[2]);
    baseLatency = Number(process.argv[3]);
    latencyFactor = Number(process.argv[4]);
}

async function initMessaging() {
    connection = await amqplib.connect('amqp://localhost');
    channel = await connection.createChannel();

    await channel.assertQueue(config.sailorInitQueue);

    await channel.assertExchange(config.shipQueue, 'fanout');

    shipQueue = await channel.assertQueue('', { exclusive: true, durable: true });
    await channel.bindQueue(shipQueue.queue, config.shipQueue, '');
}

async function handleIdentification() {
    let resolve: (value?: unknown) => void;

    await channel.consume(config.sailorInitQueue, async (msg) => {
        if (id === undefined) {
            id = Number(msg?.content.toString());

            if (msg) {
                channel.ack(msg);
            }

            await channel.cancel(msg?.fields?.consumerTag as string);

            console.log(`Friend #${id} ready - latency ${baseLatency * latencyFactor}`);

            resolve();
        } else {
            if (msg) {
                channel.reject(msg);
            }
        }
    });

    return new Promise((mResolve) => resolve = mResolve);
}

async function initSailersMessaging() {
    for (let i = 0; i < config.maxSailors; i++) {
        if (i !== id) {
            await channel.assertQueue(config.targetQueuePrefix + i);
        }
    }
}

async function listenForcastRequests() {
    await channel.consume(shipQueue.queue, async (msg) => {
        if (msg && !msg?.properties?.replyTo) {
            channel.ack(msg);
            return;
        }

        const messageRaw: any = msg?.content.toString();
        const message = JSON.parse(messageRaw);

        console.log(`Friend #${id} got forcast request`);

        ships.add(msg?.properties?.replyTo);

        const forcast = await generateForcast();

        if (ships.has(msg?.properties?.replyTo)) {
            console.log(`Friend #${id} made the forcast - ${forcast}`);

            for (let i = 0; i < config.maxSailors; i++) {
                if (i !== id) {
                    channel.sendToQueue(config.targetQueuePrefix + i, Buffer.from(forcast), { replyTo: msg?.properties?.replyTo });
                }
            }

            channel.sendToQueue(msg?.properties?.replyTo, Buffer.from(JSON.stringify({ requestId: message.requestId, friendId: id, forcast })));
        }

        if (msg) {
            channel.ack(msg);
        }
    });
}

async function listenToSailors() {
    await channel.consume(config.targetQueuePrefix + id, async (msg) => {
        console.log(`Friend ${id} - got acknoledged of another forcast`);
        if (msg && !msg?.properties?.replyTo) {
            channel.ack(msg);
            return;
        }

        if (ships.has(msg?.properties?.replyTo)) {
            ships.delete(msg?.properties?.replyTo);
        }

        if (msg) {
            channel.ack(msg);
        }
    });
}

async function generateForcast(): Promise<string> {
    // const generationTime = Math.random() * 1000 + 1000;

    const hash = crypto.createHash('md5').update(String(generateForcast) + String(id)).digest('hex');

    return new Promise((resolve) => setTimeout(() => resolve(new Date().toISOString()), baseLatency * latencyFactor));
}

run();
