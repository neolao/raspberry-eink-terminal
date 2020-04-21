const gd = require('node-gd');
const epd = require("./build/Release/epd7x5");

const width = 800;
const height = 480;

function displayImageBuffer(img){
  const buf = Buffer.alloc(width * height, 0x00);
  //const buf = Buffer.alloc(width * height, 0xFF);

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++){
      const pixel = img.getPixel(x, y);
      if (pixel > 0) {
        const i = Math.floor((y * width + x) / 8);
        buf[i] |= 0x80 >> (x % 8);
        //buf[i] &= ~(0x80 >> (x % 8));
      }
    }
  }
  epd.display(buf);
}

async function main() {
  epd.init();
  //epd.clear();

  const font = `${__dirname}/fonts/DejaVuSansMono.ttf`;
  const fontSize = 10;
  const maxCol = Math.floor(width / (fontSize / 1.25));
  const maxRow = Math.floor(height / fontSize);

  const img = await gd.create(width, height);
  const white = img.colorAllocate(255, 255, 255);
  const black = img.colorAllocate(0, 0, 0);
  img.rectangle(0, 0, width, height, white);
  img.stringFT(black, font, fontSize, 0, 0, 16, `Screen size: ${maxCol} x ${maxRow}`);
  img.stringFT(black, font, fontSize, 0, 0, fontSize * maxRow, " ".repeat(maxCol - 1) + "┘");
  //img.rectangle(20, 20, 280, 180, black);

  displayImageBuffer(img);

  img.destroy();
}
main();
