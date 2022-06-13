import * as readline from 'readline';

import amqplib from 'amqplib';

const requestQueueName = 'geo.request';

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

const response: any = {};
let total: number = 0;

async function run() {
    const connection = await amqplib.connect('amqp://localhost');
    const channel = await connection.createChannel();

    await channel.assertQueue(requestQueueName);
    const responseQueue = await channel.assertQueue('');

    console.log(`Message channel & queues ready`);

    await channel.consume(responseQueue.queue, (msg) => {
        const message: any = JSON.parse(msg?.content.toString() as string);
        const city = message?.payload?.city;
        const street = message?.payload?.street;
        const finished = message?.payload?.finished;

        if (city && street) {
            console.log(`${city}, ${street}`);
            total++;
            if (response[city]) {
                response[city].push(street);
            } else {
                response[city] = [street];
            }
        } else if (finished) {
            console.log('-----------------');
            for (const cityIt in response) {
                console.log(`[${cityIt}]`);
                for (const streetIt of response[cityIt]) {
                    console.log(streetIt);
                }
                console.log(response[cityIt].length);
            }
            process.exit(0);
        } else {
            throw new Error('Wrong payload structure');
        }

        if (msg) {
            channel.ack(msg);
        }
    });

    const letter = await new Promise((resolve) => {
        rl.question('Enter letter: ', (answer) => {
            rl.close();
            resolve(answer);
        });
    });

    channel.sendToQueue(
        requestQueueName,
        Buffer.from(JSON.stringify({ payload: { letter } })),
        { replyTo: responseQueue.queue }
    );
}

run();
