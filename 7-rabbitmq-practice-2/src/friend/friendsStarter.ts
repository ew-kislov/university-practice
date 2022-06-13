import { spawn } from 'child_process';

import amqplib from 'amqplib';

import config from './config.json';

async function run() {
    const connection = await amqplib.connect('amqp://localhost');
    const channel = await connection.createChannel();

    await channel.assertQueue(config.sailorInitQueue);

    for (let i = 0; i < config.maxSailors; i++) {
        channel.sendToQueue(config.sailorInitQueue, Buffer.from(String(i)));

        const worker = spawn(
            `npm`,
            [`run`, `start:friend:dev`, String(i), String(config.baseLatencyMs), String(config.sailorLatencyDistribution[i])]
        );

        worker.stdout.on('data', (data) => data.toString()[1] !== '>' ? console.log(data.toString().slice(0, -1)) : null);
    }
}

run();
