const gd = require('node-gd');
const epd = require("./build/Release/epd7x5");

const width = 800;
const height = 480;

function imgToBuff(img) {
  const buf = Buffer.alloc(width * height, 0x00);

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++){
      const pixel = img.getPixel(x, y);
      if (pixel > 0) {
        const i = Math.floor((y * width + x) / 8);
        buf[i] |= 0x80 >> (x % 8);
      }
    }
  }

  return buf;
}

function displayImageBuffer(img){
  const buf = imgToBuff(img);
  epd.display(buf);
}

async function main() {
  epd.init();
  epd.clear();

  const font = `${__dirname}/fonts/DejaVuSansMono.ttf`;
  const fontSize = 60;
  const maxCol = Math.floor(width / (fontSize / 1.25));
  const maxRow = Math.floor(height / fontSize);

  const img = await gd.create(width, height);
  const white = img.colorAllocate(255, 255, 255);
  const black = img.colorAllocate(0, 0, 0);
  img.rectangle(0, 0, width, height, white);

  let message = "";
  for (let i = 0; i < 5; i++) {
    message += i;
    console.log("Message:", message);
    img.stringFT(black, font, fontSize, 0, 50, 200, message);
    //displayImageBuffer(img);

    epd.setPartialWindow(imgToBuff(img), 0, 0, width, height);
    epd.setLutQuick();

    epd.refresh();
    epd.waitUntilIdle();
  }


  img.destroy();
}
main();
