import * as fs from 'fs';

class Csomopont {
    betu: string;
    balNulla: Csomopont | null;
    jobbEgy: Csomopont | null;

    constructor(b: string = '/') {
        this.betu = b;
        this.balNulla = null;
        this.jobbEgy = null;
    }
}

class LZWBinFa {
    gyoker: Csomopont;
    fa: Csomopont;
    melyseg: number = 0;
    maxMelyseg: number = 0;
    atlagosszeg: number = 0;
    szorasosszeg: number = 0;
    atlagdb: number = 0;
    atlag: number = 0;
    szoras: number = 0;

    constructor() {
        this.gyoker = new Csomopont('/');
        this.fa = this.gyoker;
    }

    push(b: string) {
        if (b === '0') {
            if (!this.fa.balNulla) {
                this.fa.balNulla = new Csomopont('0');
                this.fa = this.gyoker;
            } else {
                this.fa = this.fa.balNulla;
            }
        } else {
            if (!this.fa.jobbEgy) {
                this.fa.jobbEgy = new Csomopont('1');
                this.fa = this.gyoker;
            } else {
                this.fa = this.fa.jobbEgy;
            }
        }
    }

    kiir(fd: number) {
        this.melyseg = 0;
        this._kiir(this.gyoker, fd);
    }

    private _kiir(elem: Csomopont | null, fd: number) {
        if (elem !== null) {
            this.melyseg++;
            this._kiir(elem.jobbEgy, fd);
            for (let i = 0; i < this.melyseg; i++) {
                fs.writeSync(fd, "---");
            }
            fs.writeSync(fd, `${elem.betu}(${this.melyseg - 1})\n`);
            this._kiir(elem.balNulla, fd);
            this.melyseg--;
        }
    }

    getMelyseg(): number {
        this.melyseg = 0;
        this.maxMelyseg = 0;
        this.rmelyseg(this.gyoker);
        return this.maxMelyseg - 1;
    }

    private rmelyseg(elem: Csomopont | null) {
        if (elem !== null) {
            this.melyseg++;
            if (this.melyseg > this.maxMelyseg) {
                this.maxMelyseg = this.melyseg;
            }
            this.rmelyseg(elem.jobbEgy);
            this.rmelyseg(elem.balNulla);
            this.melyseg--;
        }
    }

    getAtlag(): number {
        this.melyseg = 0;
        this.atlagosszeg = 0;
        this.atlagdb = 0;
        this.ratlag(this.gyoker);
        this.atlag = this.atlagdb > 0 ? this.atlagosszeg / this.atlagdb : 0;
        return this.atlag;
    }

    private ratlag(elem: Csomopont | null) {
        if (elem !== null) {
            this.melyseg++;
            this.ratlag(elem.jobbEgy);
            this.ratlag(elem.balNulla);
            this.melyseg--;
            if (elem.jobbEgy === null && elem.balNulla === null) {
                this.atlagdb++;
                this.atlagosszeg += this.melyseg;
            }
        }
    }

    getSzoras(): number {
        this.atlag = this.getAtlag();
        this.szorasosszeg = 0.0;
        this.melyseg = 0;
        this.atlagdb = 0;
        this.rszoras(this.gyoker);

        if (this.atlagdb - 1 > 0) {
            this.szoras = Math.sqrt(this.szorasosszeg / (this.atlagdb - 1));
        } else {
            this.szoras = Math.sqrt(this.szorasosszeg);
        }
        return this.szoras;
    }

    private rszoras(elem: Csomopont | null) {
        if (elem !== null) {
            this.melyseg++;
            this.rszoras(elem.jobbEgy);
            this.rszoras(elem.balNulla);
            this.melyseg--;
            if (elem.jobbEgy === null && elem.balNulla === null) {
                this.atlagdb++;
                this.szorasosszeg += Math.pow(this.melyseg - this.atlag, 2);
            }
        }
    }
}

function usage() {
        console.log("Usage: lzwtree in_file -o out_file");
}

function formatNumber(num: number): string {
    // Format to 6 significant digits to match C++'s default output (%g)
    return parseFloat(num.toPrecision(6)).toString();
}

function main() {
    const args = process.argv.slice(2);
    if (args.length !== 3) {
        usage();
        process.exit(-1);
    }

    const inFile = args[0];
    if (args[1] !== '-o') {
        usage();
        process.exit(-2);
    }
    const outFile = args[2];

    let data: Buffer;
    try {
        data = fs.readFileSync(inFile);
    } catch (err) {
        console.log(`${inFile} nem letezik...`);
        usage();
        process.exit(-3);
    }

    // Skip the first line as in original C++ impl loop condition
    let startIdx = 0;
    for (let i = 0; i < data.length; i++) {
        if (data[i] === 0x0a) {
            startIdx = i + 1;
            break;
        }
    }

    const binFa = new LZWBinFa();
    let kommentben = false;

    for (let i = startIdx; i < data.length; i++) {
        let b = data[i];

        if (b === 0x3e) { // '>'
            kommentben = true;
            continue;
        }
        if (b === 0x0a) { // '\n'
            kommentben = false;
            continue;
        }
        if (kommentben) continue;
        if (b === 0x4e) continue; // 'N'

        for (let j = 0; j < 8; j++) {
            if (b & 0x80) {
                binFa.push('1');
            } else {
                binFa.push('0');
            }
            b = (b << 1) & 0xff;
        }
    }

    const fd = fs.openSync(outFile, 'w');
    binFa.kiir(fd);
    fs.writeSync(fd, `depth = ${binFa.getMelyseg()}\n`);
    fs.writeSync(fd, `mean = ${formatNumber(binFa.getAtlag())}\n`);
    fs.writeSync(fd, `var = ${formatNumber(binFa.getSzoras())}\n`);
    fs.closeSync(fd);
}

main();