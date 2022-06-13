import { spawn } from 'child_process';
import fsp from 'fs/promises';
import path from 'path';

import amqplib from 'amqplib';

const requestQueueName = 'geo.request';
const workerInitQueueName = 'geo.init-worker';
const responseQueueName = 'geo.worker-response';
const letterPublishingExchange = 'geo.letter-pub';

let connection: amqplib.Connection;
let channel: amqplib.Channel;

let workersNum: number;
let activeWorkers: number = 0;

const clientStates: any = {};

async function run() {
    workersNum = process.env.WORKERS_NUM ? Number(process.env.WORKERS_NUM) : 3;

    console.log(`Starting master`);
    console.log(`Workers using: ${workersNum}`);

    connection = await amqplib.connect('amqp://localhost');
    channel = await connection.createChannel();

    await channel.assertQueue(requestQueueName);
    await channel.assertQueue(workerInitQueueName);
    await channel.assertQueue(responseQueueName);
    await channel.assertExchange(letterPublishingExchange, 'fanout', { durable: true });

    console.log(`Message channel & queues ready`);

    await initWorkers();

    console.log(`Workers initiated`);

    await channel.consume(responseQueueName, async (msg) => {
        try {
            const message: any = JSON.parse(msg?.content.toString() as string);
            const city = message?.payload?.city;
            const street = message?.payload?.street;
            const error = message?.payload?.error;
            const finished = message?.payload?.finished;

            const replyTo: string = msg?.properties.replyTo;

            if (!replyTo) {
                throw new Error('replyTo property should be specified');
            }

            if (error) {
                activeWorkers--;
            } else if (city && street) {
                channel.sendToQueue(replyTo, Buffer.from(JSON.stringify({ payload: { city, street } })));
            } else if (finished) {
                if (clientStates[replyTo]) {
                    clientStates[replyTo]++;
                } else {
                    clientStates[replyTo] = 1;
                }
                if (clientStates[replyTo] === activeWorkers) {
                    channel.sendToQueue(replyTo, Buffer.from(JSON.stringify({ payload: { finished: true } })));
                }
            }
        } catch (error) {
            console.error(error);
        } finally {
            if (msg) {
                channel.ack(msg);
            }
        }
    });

    await channel.consume(requestQueueName, async (msg) => {
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

            console.log(`Got letter from client - ${letter}`);
            await sendLetterToWorkers(letter, replyTo);
        } catch (error: any) {
            console.error(error.message);
        } finally {
            if (msg) {
                channel.ack(msg);
            }
        }
    });
}

async function initWorkers() {
    const files = await fsp.readdir(path.join(__dirname, '../../data-osm'));
    const states = files.map((file) => file.split('.')[0]);

    const statesPerWorkerFloat = states.length / workersNum;
    const statesTail = states.length % workersNum;
    const statesPerWorker: number =
        statesTail > (Math.ceil(statesPerWorkerFloat) - states.length % Math.ceil(statesPerWorkerFloat)) ?
            Math.ceil(statesPerWorkerFloat) : Math.floor(statesPerWorkerFloat);

    for (let i = 0; i < workersNum; i++) {
        const workerStates = i === workersNum - 1 ?
            states.slice(i * statesPerWorker, states.length)
            : states.slice(i * statesPerWorker, (i + 1) * statesPerWorker);

        const worker = spawn(`npm`, [`run`, `start:worker:dev`]);

        worker.stdout.on('data', (data) => data.toString()[1] !== '>' ? console.log(`Worker #${i + 1}: ${data.toString().slice(0, -1)}`) : null);
        worker.stdout.on('error', (data) => console.log(`Worker #${i + 1}: Error: ${data.toString().substring(0, -1)}`));
        worker.on('exit', (code) => console.log(`Worker #${i + 1} terminated with code ${code?.toString()}`));

        channel.sendToQueue(workerInitQueueName, Buffer.from(JSON.stringify({ payload: { states: workerStates } })));

        activeWorkers++;
    }
}

async function sendLetterToWorkers(letter: string, replyTo: string) {
    const message = { payload: { letter } };
    channel.publish(letterPublishingExchange, '', Buffer.from(JSON.stringify(message)), { replyTo });
}

run();
