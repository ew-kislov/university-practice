import amqplib from 'amqplib';

import config from './config.json';

let requestId = 0;

async function run() {
    const connection = await amqplib.connect('amqp://localhost');
    const channel = await connection.createChannel();

    const responseQueue = await channel.assertQueue('');

    await channel.assertExchange(config.shipQueue, 'fanout', { durable: true });

    await channel.consume(responseQueue.queue, (msg) => {
        const messageRaw: any = msg?.content.toString();
        const message = JSON.parse(messageRaw);

        if (message.requestId === requestId) {
            console.log(`Friend #${message.friendId} send forcast - ${message.forcast}`);
        }
    });

    setInterval(() => {
        requestId++;
        channel.publish(config.shipQueue, '', Buffer.from(JSON.stringify({ requestId })), { replyTo: responseQueue.queue });
        console.log(`Sent request ${requestId}`);
    }, config.maxWaitingTimeMs);
}

run();
