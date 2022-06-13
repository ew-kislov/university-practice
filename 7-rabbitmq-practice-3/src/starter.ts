import { spawn } from 'child_process';

import config from '../config.json';

async function run() {
    for (let i = 1; i <= config.totalSailors; i++) {
        const worker = spawn(
            `npm`,
            [`run`, `start:sailor:dev`, String(i)]
        );

        worker.stdout.on('data', (data) => data.toString()[1] !== '>' ? console.log(data.toString().slice(0, -1)) : null);
    }
}

run();
