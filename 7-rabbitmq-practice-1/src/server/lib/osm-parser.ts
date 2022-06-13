import fs from 'fs';

const XmlStream = require('xml-stream');

export interface StreetObject {
    city: string;
    street: string;
}

export interface OsmParserParams {
    filePath: string;
    streetCallback: (streetObject: StreetObject, stopParsing: () => void) => void;
    immediateCallback?: (stopParsing: () => void) => void;
    endCallback?: () => void;
}

export async function parseOsm(params: OsmParserParams): Promise<any> {
    const fileStream = fs.createReadStream(params.filePath);
    const xmlStream = new XmlStream(fileStream);

    const pauseStream = () => xmlStream.pause();

    const foundStreets: string[] = [];

    xmlStream.collect('tag');

    xmlStream.on('endElement: node', (node: any) => {
        const cityTag = node.tag ? node.tag.find(cityTagFilter) : null;
        const streetTag = node.tag ? node.tag.find(streetTagFilter) : null;
        if (cityTag && streetTag) {
            const city = cityTag.$.v;
            const street = streetTag.$.v;

            const streetObject: StreetObject = { city, street };

            if (!foundStreets.includes(streetObject.street)) {
                foundStreets.push(streetObject.street);
                params.streetCallback(streetObject, pauseStream);
            }
        }
    });

    xmlStream.on('startElement: tag', (node: any) => {
        if (params.immediateCallback) {
            params.immediateCallback(pauseStream);
        }
    });

    xmlStream.on('startElement: node', (node: any) => {
        if (params.immediateCallback) {
            params.immediateCallback(pauseStream);
        }
    });

    xmlStream.on('end', async () => {
        if (params.endCallback) {
            params.endCallback();
        }
    });
}

function cityTagFilter(attribute: any) {
    return attribute.$.k === 'addr:city';
}

function streetTagFilter(attribute: any) {
    return attribute.$.k === 'addr:street';
}
