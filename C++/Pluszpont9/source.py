import sys
import math

class Csomopont:
    def __init__(self, b='/'):
        self.betu = b
        self.balNulla = None
        self.jobbEgy = None

class LZWBinFa:
    def __init__(self):
        self.gyoker = Csomopont('/')
        self.fa = self.gyoker
        self.melyseg = 0
        self.maxMelyseg = 0
        self.atlagosszeg = 0
        self.szorasosszeg = 0.0
        self.atlagdb = 0
        self.atlag = 0.0
        self.szoras = 0.0

    def push(self, b):
        if b == '0':
            if not self.fa.balNulla:
                self.fa.balNulla = Csomopont('0')
                self.fa = self.gyoker
            else:
                self.fa = self.fa.balNulla
        else:
            if not self.fa.jobbEgy:
                self.fa.jobbEgy = Csomopont('1')
                self.fa = self.gyoker
            else:
                self.fa = self.fa.jobbEgy

    def kiir(self, os):
        self.melyseg = 0
        self._kiir(self.gyoker, os)

    def _kiir(self, elem, os):
        if elem is not None:
            self.melyseg += 1
            self._kiir(elem.jobbEgy, os)
            for _ in range(self.melyseg):
                os.write("---")
            os.write(f"{elem.betu}({self.melyseg - 1})\n")
            self._kiir(elem.balNulla, os)
            self.melyseg -= 1

    def getMelyseg(self):
        self.melyseg = 0
        self.maxMelyseg = 0
        self.rmelyseg(self.gyoker)
        return self.maxMelyseg - 1

    def rmelyseg(self, elem):
        if elem is not None:
            self.melyseg += 1
            if self.melyseg > self.maxMelyseg:
                self.maxMelyseg = self.melyseg
            self.rmelyseg(elem.jobbEgy)
            self.rmelyseg(elem.balNulla)
            self.melyseg -= 1

    def getAtlag(self):
        self.melyseg = 0
        self.atlagosszeg = 0
        self.atlagdb = 0
        self.ratlag(self.gyoker)
        self.atlag = self.atlagosszeg / self.atlagdb if self.atlagdb > 0 else 0
        return self.atlag

    def ratlag(self, elem):
        if elem is not None:
            self.melyseg += 1
            self.ratlag(elem.jobbEgy)
            self.ratlag(elem.balNulla)
            self.melyseg -= 1
            if elem.jobbEgy is None and elem.balNulla is None:
                self.atlagdb += 1
                self.atlagosszeg += self.melyseg

    def getSzoras(self):
        self.atlag = self.getAtlag()
        self.szorasosszeg = 0.0
        self.melyseg = 0
        self.atlagdb = 0
        self.rszoras(self.gyoker)
        
        if self.atlagdb - 1 > 0:
            self.szoras = math.sqrt(self.szorasosszeg / (self.atlagdb - 1))
        else:
            self.szoras = math.sqrt(self.szorasosszeg)
        return self.szoras

    def rszoras(self, elem):
        if elem is not None:
            self.melyseg += 1
            self.rszoras(elem.jobbEgy)
            self.rszoras(elem.balNulla)
            self.melyseg -= 1
            if elem.jobbEgy is None and elem.balNulla is None:
                self.atlagdb += 1
                self.szorasosszeg += (self.melyseg - self.atlag) ** 2


def usage():
    print("Usage: lzwtree in_file -o out_file")

def main():
    if len(sys.argv) != 4:
        usage()
        sys.exit(-1)

    in_file = sys.argv[1]
    if sys.argv[2] != "-o":
        usage()
        sys.exit(-2)
    out_file = sys.argv[3]

    try:
        with open(in_file, "rb") as f_in:
            data = f_in.read()
    except IOError:
        print(f"{in_file} nem letezik...")
        usage()
        sys.exit(-3)

    # find first newline
    start_idx = 0
    while start_idx < len(data):
        if data[start_idx] == 0x0a:
            start_idx += 1
            break
        start_idx += 1

    binFa = LZWBinFa()
    kommentben = False

    for i in range(start_idx, len(data)):
        b = data[i]
        if b == 0x3e:  # '>'
            kommentben = True
            continue
        if b == 0x0a:  # '\n'
            kommentben = False
            continue
        if kommentben:
            continue
        if b == 0x4e:  # 'N'
            continue
        
        for j in range(8):
            if b & 0x80:
                binFa.push('1')
            else:
                binFa.push('0')
            b = (b << 1) & 0xff

    with open(out_file, "w") as f_out:
        binFa.kiir(f_out)
        f_out.write(f"depth = {binFa.getMelyseg()}\n")
        
        mean_val = binFa.getAtlag()
        mean_str = f"{mean_val:g}"
        if "." not in mean_str and "e" not in mean_str.lower() and mean_val != int(mean_val):
             mean_str = f"{mean_val:.5f}".rstrip("0").rstrip(".")

        var_val = binFa.getSzoras()
        
        f_out.write(f"mean = {mean_val:g}\n")
        f_out.write(f"var = {var_val:g}\n")


if __name__ == "__main__":
    main()
