import path from 'path';
import fsp from 'fs/promises';
import amqplib from 'amqplib';

import { StreetObject, parseOsm } from './lib/osm-parser';

const ADDRESSES_COUNT = 5;

const workerInitQueueName = 'geo.init-worker';
const letterPublishingExchange = 'geo.letter-pub';
const responseQueueName = 'geo.worker-response';

let letterSubscriptionQueue: string;

let connection: amqplib.Connection;
let channel: amqplib.Channel;

let areStatesInitiated: boolean = false;
let states: string[] = [];

async function run() {
    connection = await amqplib.connect('amqp://localhost');
    channel = await connection.createChannel();

    await channel.assertQueue(workerInitQueueName);
    await channel.assertQueue(responseQueueName);
    await channel.assertExchange(letterPublishingExchange, 'fanout');

    const letterSubQueue = await channel.assertQueue('', { exclusive: true, durable: true });
    await channel.bindQueue(letterSubQueue.queue, letterPublishingExchange, '');
    letterSubscriptionQueue = letterSubQueue.queue;

    console.log('Message channel & queues ready');

    channel.consume(workerInitQueueName, async (msg) => {
        try {
            if (!areStatesInitiated) {
                const message: any = JSON.parse(msg?.content.toString() as string);
                const incomingStates = message?.payload?.states;

                if (!incomingStates || isNaN(incomingStates.length)) {
                    throw new Error('Error: payload should contain states array');
                }

                states = incomingStates;
                areStatesInitiated = true;

                if (msg) {
                    channel.ack(msg);
                }
                await channel.cancel(msg?.fields?.consumerTag as string);

                if (states.length === 0) {
                    console.log('Empty states passed');
                    process.exit(0);
                }

                console.log(`Using states - ${JSON.stringify(states)}`);

                startLettersHandler();
            } else {
                if (msg) {
                    channel.reject(msg);
                }
            }
        } catch (error) {
            if (msg) {
                channel.ack(msg);
            }

            console.error(error);
        }
    });
}

function startLettersHandler() {
    channel.consume(letterSubscriptionQueue, async (msg: any) => {
        try {
            const message: any = JSON.parse(msg?.content.toString() as string);
            const letter = message?.payload?.letter;

            const replyTo: string = msg?.properties.replyTo;

            if (!replyTo) {
                throw new Error('replyTo property should be specified');
            }

            if (!letter || letter.length !== 1 || !letter.match(/[a-z]/i)) {
                throw new Error('Payload should contain letter');
            }

            console.log(`Handling letter "${letter}"`);

            await handleLetter(letter, replyTo);

            console.log(`Finished handling letter "${letter}"`);
        } catch (error: any) {
            console.error(error.message);

            if (msg) {
                channel.ack(msg);
            }
        }
    });
}

function handleLetter(letter: string, replyTo: string) {
    if (states.length === 0) {
        channel.sendToQueue(
            responseQueueName,
            Buffer.from(JSON.stringify({ payload: { finished: true } })),
            { replyTo }
        );
        return;
    }

    let finishedCallbacks = 0;
    let count = 0;

    let resolvePromise: (value?: unknown) => void;
    const promise = new Promise((resolve) => resolvePromise = resolve);

    states.forEach(async (state) => {
        const osmPath = path.join(__dirname, '../../data-osm', `${state}.osm`);

        try {
            await fsp.access(osmPath);
        } catch (error) {
            channel.sendToQueue(
                responseQueueName,
                Buffer.from(JSON.stringify({ payload: { error: true } })),
                { replyTo }
            );
            console.error(`File ${path} doesn't exist`);
            process.exit(1);
        }

        parseOsm({
            filePath: osmPath,
            streetCallback: (streetObject: StreetObject) => {
                if (streetObject.street[0].toLowerCase() === letter.toLowerCase()) {
                    channel.sendToQueue(
                        responseQueueName,
                        Buffer.from(JSON.stringify({ payload: { city: streetObject.city, street: streetObject.street } })),
                        { replyTo }
                    );
                    count++;
                }
            },
            immediateCallback: (stopParse) => {
                if (count >= ADDRESSES_COUNT) {
                    stopParse();
                    finishedCallbacks++;
                    if (finishedCallbacks === states.length) {
                        channel.sendToQueue(
                            responseQueueName,
                            Buffer.from(JSON.stringify({ payload: { finished: true } })),
                            { replyTo }
                        );
                        resolvePromise();
                    }
                }
            }
        });
    });

    return promise;
}

run();
