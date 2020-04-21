const gd = require('node-gd');
const epd = require("./build/Release/epd7x5");

const width = 800;
const height = 480;

function displayImageBuffer(img){
  const buf = Buffer.alloc(width * height, 0x00);

  for (let y = 0; y < height; y++) {
    for (let x = 0; x < width; x++){
      const pixel = img.getPixel(x, y);
      if (pixel > 0) {
        const i = Math.round((y * width + x) / 8);
        buf[i] = 0xFF;
      }
    }
  }
  epd.display(buf);
}


async function main() {
  epd.init();
  //epd.clear();

  const font = `${__dirname}/fonts/DejaVuSansMono.ttf`;

  const img = await gd.create(width, height);
  const white = img.colorAllocate(255, 255, 255);
  const black = img.colorAllocate(0, 0, 0);
  img.rectangle(0, 0, width, height, white);
  const textColor = img.colorAllocate(0, 0, 0);
  img.stringFT(textColor, font, 64, 0, 300, 60, 'Hello');
  img.setPixel(51, 51, 0x000000);
  img.rectangle(20, 20, 280, 180, black);

  displayImageBuffer(img);

  await img.savePng(`${__dirname}/file.png`, 0);
  img.destroy();

//epd.clear();
}
main();
