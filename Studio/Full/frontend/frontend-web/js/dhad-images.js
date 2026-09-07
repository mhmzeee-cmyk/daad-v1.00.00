// ═══════════════════════════════════════════════════════════════════════════════
// Dhad (ض) Language — Image Processing Library
// Supports 20 image formats for both cloud editor and desktop
// ═══════════════════════════════════════════════════════════════════════════════

var DhadImages = (function () {
  'use strict';

  // ── Supported Image Formats (20) ────────────────────────────────────────────
  var SUPPORTED_FORMATS = {
    'jpg':  { mime: 'image/jpeg',        ext: '.jpg',  name: 'JPEG' },
    'jpeg': { mime: 'image/jpeg',        ext: '.jpeg', name: 'JPEG' },
    'png':  { mime: 'image/png',         ext: '.png',  name: 'PNG' },
    'gif':  { mime: 'image/gif',         ext: '.gif',  name: 'GIF' },
    'bmp':  { mime: 'image/bmp',         ext: '.bmp',  name: 'BMP' },
    'tiff': { mime: 'image/tiff',        ext: '.tiff', name: 'TIFF' },
    'tif':  { mime: 'image/tiff',        ext: '.tif',  name: 'TIFF' },
    'webp': { mime: 'image/webp',        ext: '.webp', name: 'WebP' },
    'svg':  { mime: 'image/svg+xml',     ext: '.svg',  name: 'SVG' },
    'ico':  { mime: 'image/x-icon',      ext: '.ico',  name: 'ICO' },
    'hdr':  { mime: 'image/vnd.radiance', ext: '.hdr', name: 'HDR' },
    'tga':  { mime: 'image/x-tga',       ext: '.tga',  name: 'TGA' },
    'psd':  { mime: 'image/vnd.adobe.photoshop', ext: '.psd', name: 'PSD' },
    'raw':  { mime: 'image/raw',         ext: '.raw',  name: 'RAW' },
    'heic': { mime: 'image/heic',        ext: '.heic', name: 'HEIC' },
    'heif': { mime: 'image/heif',        ext: '.heif', name: 'HEIF' },
    'avif': { mime: 'image/avif',        ext: '.avif', name: 'AVIF' },
    'jxl':  { mime: 'image/jxl',         ext: '.jxl',  name: 'JPEG XL' },
    'pnm':  { mime: 'image/x-portable-anymap', ext: '.pnm', name: 'PNM' },
    'qoi':  { mime: 'image/qoi',         ext: '.qoi',  name: 'QOI' },
    'dds':  { mime: 'image/vnd.ms-directx', ext: '.dds', name: 'DDS' },
    'exr':  { mime: 'image/x-exr',       ext: '.exr',  name: 'EXR' }
  };

  // ── Internal State ──────────────────────────────────────────────────────────
  var canvas = null;
  var ctx = null;
  var images = {}; // loaded images cache

  // ── Canvas Management ───────────────────────────────────────────────────────
  function ensureCanvas(width, height) {
    if (!canvas) {
      canvas = document.createElement('canvas');
      canvas.id = 'dhad-canvas';
      canvas.width = width || 800;
      canvas.height = height || 600;
      canvas.style.border = '2px solid #333';
      canvas.style.backgroundColor = '#000';
      canvas.style.display = 'block';
      // Insert canvas into page
      var container = document.getElementById('dhad-output') || document.body;
      container.appendChild(canvas);
      ctx = canvas.getContext('2d');
    }
    return canvas;
  }

  // ── Format Detection ────────────────────────────────────────────────────────
  function detectFormat(path) {
    var ext = path.split('.').pop().toLowerCase();
    return SUPPORTED_FORMATS[ext] || null;
  }

  function isFormatSupported(path) {
    return detectFormat(path) !== null;
  }

  function getSupportedFormats() {
    return Object.keys(SUPPORTED_FORMATS);
  }

  // ── Image Loading ───────────────────────────────────────────────────────────
  // حمّل_صورة("path", varName)
  function loadImage(path) {
    return new Promise(function (resolve, reject) {
      var format = detectFormat(path);
      if (!format) {
        console.error('[ض] صيغة الصورة غير مدعومة: ' + path);
        console.error('[ض] الصيغ المدعومة: ' + getSupportedFormats().join(', '));
        reject(new Error('Unsupported format: ' + path));
        return;
      }

      var img = new Image();
      img.crossOrigin = 'anonymous';
      img.onload = function () {
        images[path] = img;
        ensureCanvas(Math.max(canvas ? canvas.width : 800, img.width), Math.max(canvas ? canvas.height : 600, img.height));
        console.log('[ض] تم تحميل الصورة: ' + path + ' (' + format.name + ' ' + img.width + 'x' + img.height + ')');
        resolve(img);
      };
      img.onerror = function () {
        console.error('[ض] فشل تحميل الصورة: ' + path);
        reject(new Error('Failed to load image: ' + path));
      };
      img.src = path;
    });
  }

  // ── Image Drawing ───────────────────────────────────────────────────────────
  // ارسم_صورة(imgVar, x, y, w, h)
  function drawImage(img, x, y, w, h) {
    if (!img || !img.width) {
      console.error('[ض] صورة غير صالحة للرسم');
      return;
    }
    ensureCanvas();
    if (w && h) {
      ctx.drawImage(img, x, y, w, h);
    } else {
      ctx.drawImage(img, x, y);
    }
  }

  // ── Image Size ──────────────────────────────────────────────────────────────
  // حجم_صورة(imgVar)
  function imageSize(img) {
    if (!img) return { width: 0, height: 0 };
    return { width: img.width, height: img.height };
  }

  // ── Image Saving ────────────────────────────────────────────────────────────
  // احفظ_صورة(imgVar, "path")
  function saveImage(img, path) {
    if (!canvas) {
      console.error('[ض] لا توجد لوحة رسوم متحركة');
      return;
    }
    var format = detectFormat(path);
    if (!format) {
      console.error('[ض] صيغة غير مدعومة للحفظ: ' + path);
      return;
    }
    // Draw the image to canvas first if needed
    if (img && img !== canvas) {
      ctx.drawImage(img, 0, 0);
    }
    var dataURL = canvas.toDataURL(format.mime);
    var link = document.createElement('a');
    link.download = path.split('/').pop();
    link.href = dataURL;
    link.click();
    console.log('[ض] تم حفظ الصورة: ' + path);
  }

  // ── Image Cropping ──────────────────────────────────────────────────────────
  // قص_صورة(imgVar, x, y, w, h)
  function cropImage(img, x, y, w, h) {
    if (!img) return null;
    var tempCanvas = document.createElement('canvas');
    tempCanvas.width = w;
    tempCanvas.height = h;
    var tempCtx = tempCanvas.getContext('2d');
    tempCtx.drawImage(img, x, y, w, h, 0, 0, w, h);
    var cropped = new Image();
    cropped.src = tempCanvas.toDataURL();
    return cropped;
  }

  // ── Image Resize ────────────────────────────────────────────────────────────
  // غيّر_حجم(imgVar, w, h)
  function resize(img, w, h) {
    if (!img) return null;
    var tempCanvas = document.createElement('canvas');
    tempCanvas.width = w;
    tempCanvas.height = h;
    var tempCtx = tempCanvas.getContext('2d');
    tempCtx.drawImage(img, 0, 0, w, h);
    var resized = new Image();
    resized.src = tempCanvas.toDataURL();
    return resized;
  }

  // ── Image Rotation ──────────────────────────────────────────────────────────
  // لف_صورة(imgVar, angle)
  function rotateImage(img, angle) {
    if (!img) return null;
    var radians = angle * Math.PI / 180;
    var cos = Math.abs(Math.cos(radians));
    var sin = Math.abs(Math.sin(radians));
    var newW = img.width * cos + img.height * sin;
    var newH = img.width * sin + img.height * cos;
    var tempCanvas = document.createElement('canvas');
    tempCanvas.width = newW;
    tempCanvas.height = newH;
    var tempCtx = tempCanvas.getContext('2d');
    tempCtx.translate(newW / 2, newH / 2);
    tempCtx.rotate(radians);
    tempCtx.drawImage(img, -img.width / 2, -img.height / 2);
    var rotated = new Image();
    rotated.src = tempCanvas.toDataURL();
    return rotated;
  }

  // ── Image Flip ──────────────────────────────────────────────────────────────
  // قلب_صورة(imgVar, "أفقي"/"عمودي")
  function flipImage(img, direction) {
    if (!img) return null;
    var tempCanvas = document.createElement('canvas');
    tempCanvas.width = img.width;
    tempCanvas.height = img.height;
    var tempCtx = tempCanvas.getContext('2d');
    if (direction === 'أفقي' || direction === 'horizontal') {
      tempCtx.translate(img.width, 0);
      tempCtx.scale(-1, 1);
    } else {
      tempCtx.translate(0, img.height);
      tempCtx.scale(1, -1);
    }
    tempCtx.drawImage(img, 0, 0);
    var flipped = new Image();
    flipped.src = tempCanvas.toDataURL();
    return flipped;
  }

  // ── Opacity ─────────────────────────────────────────────────────────────────
  // شفافية(imgVar, value)  value: 0-1
  function opacity(img, value) {
    if (!img) return;
    ensureCanvas();
    ctx.globalAlpha = value;
    ctx.drawImage(img, 0, 0);
    ctx.globalAlpha = 1.0;
  }

  // ── Filters ─────────────────────────────────────────────────────────────────
  // فلتر(imgVar, "filterName")
  function filter(img, filterName) {
    if (!img) return null;
    ensureCanvas();
    ctx.drawImage(img, 0, 0);
    var imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
    var data = imageData.data;

    switch (filterName) {
      case 'رمادي': case 'grayscale':
        for (var i = 0; i < data.length; i += 4) {
          var avg = (data[i] + data[i + 1] + data[i + 2]) / 3;
          data[i] = avg;
          data[i + 1] = avg;
          data[i + 2] = avg;
        }
        break;
      case 'عكس': case 'invert':
        for (var i = 0; i < data.length; i += 4) {
          data[i] = 255 - data[i];
          data[i + 1] = 255 - data[i + 1];
          data[i + 2] = 255 - data[i + 2];
        }
        break;
      case 'ساطع': case 'brightness':
        for (var i = 0; i < data.length; i += 4) {
          data[i] = Math.min(255, data[i] + 50);
          data[i + 1] = Math.min(255, data[i + 1] + 50);
          data[i + 2] = Math.min(255, data[i + 2] + 50);
        }
        break;
      case 'داكن': case 'darkness':
        for (var i = 0; i < data.length; i += 4) {
          data[i] = Math.max(0, data[i] - 50);
          data[i + 1] = Math.max(0, data[i + 1] - 50);
          data[i + 2] = Math.max(0, data[i + 2] - 50);
        }
        break;
      case 'حدّة': case 'sharpen':
        // Simple sharpen kernel
        var kernel = [0, -1, 0, -1, 5, -1, 0, -1, 0];
        applyKernel(imageData, kernel);
        break;
      case 'ضبابي': case 'blur':
        var blurKernel = [1/9, 1/9, 1/9, 1/9, 1/9, 1/9, 1/9, 1/9, 1/9];
        applyKernel(imageData, blurKernel);
        break;
      case 'حدود': case 'edge':
        var edgeKernel = [-1, -1, -1, -1, 8, -1, -1, -1, -1];
        applyKernel(imageData, edgeKernel);
        break;
      case 'سيبيا': case 'sepia':
        for (var i = 0; i < data.length; i += 4) {
          var r = data[i], g = data[i + 1], b = data[i + 2];
          data[i] = Math.min(255, r * 0.393 + g * 0.769 + b * 0.189);
          data[i + 1] = Math.min(255, r * 0.349 + g * 0.686 + b * 0.168);
          data[i + 2] = Math.min(255, r * 0.272 + g * 0.534 + b * 0.131);
        }
        break;
      default:
        console.warn('[ض] فلتر غير معروف: ' + filterName);
        console.warn('[ض] الفلتر المدعومة: رمادي، عكس، ساطع، داكن، حدّة، ضبابي، حدود، سيبيا');
    }

    ctx.putImageData(imageData, 0, 0);
    var filtered = new Image();
    filtered.src = canvas.toDataURL();
    return filtered;
  }

  function applyKernel(imageData, kernel) {
    var data = imageData.data;
    var w = imageData.width;
    var h = imageData.height;
    var output = new Uint8ClampedArray(data.length);

    for (var y = 1; y < h - 1; y++) {
      for (var x = 1; x < w - 1; x++) {
        for (var c = 0; c < 3; c++) {
          var val = 0;
          for (var ky = -1; ky <= 1; ky++) {
            for (var kx = -1; kx <= 1; kx++) {
              var idx = ((y + ky) * w + (x + kx)) * 4 + c;
              val += data[idx] * kernel[(ky + 1) * 3 + (kx + 1)];
            }
          }
          output[(y * w + x) * 4 + c] = Math.min(255, Math.max(0, val));
        }
        output[(y * w + x) * 4 + 3] = data[(y * w + x) * 4 + 3];
      }
    }

    for (var i = 0; i < data.length; i++) {
      data[i] = output[i];
    }
  }

  // ── Overlay ─────────────────────────────────────────────────────────────────
  //تراكب(imgVar1, imgVar2, x, y)
  function overlay(img1, img2, x, y) {
    if (!img1 || !img2) return;
    ensureCanvas();
    ctx.drawImage(img1, 0, 0);
    ctx.drawImage(img2, x, y);
  }

  // ── Background ──────────────────────────────────────────────────────────────
  // خلفية(imgVar, bgVar)
  function background(img, bgImg) {
    if (!bgImg) return;
    ensureCanvas();
    ctx.drawImage(bgImg, 0, 0, canvas.width, canvas.height);
    if (img) {
      ctx.drawImage(img, 0, 0);
    }
  }

  // ── Pixel Access ────────────────────────────────────────────────────────────
  //بكسل(imgVar, x, y) — returns {r, g, b, a}
  function pixel(img, x, y) {
    if (!img) return { r: 0, g: 0, b: 0, a: 0 };
    ensureCanvas();
    ctx.drawImage(img, 0, 0);
    var p = ctx.getImageData(x, y, 1, 1).data;
    return { r: p[0], g: p[1], b: p[2], a: p[3] };
  }

  // ── Drawing Shapes ──────────────────────────────────────────────────────────
  // ارسم("shape", args...)
  function draw(shape, args) {
    ensureCanvas();
    ctx.beginPath();
    switch (shape) {
      case 'مستطيل': case 'rect':
        ctx.strokeRect(args[0], args[1], args[2], args[3]);
        break;
      case 'دائرة': case 'circle':
        ctx.arc(args[0], args[1], args[2], 0, Math.PI * 2);
        ctx.stroke();
        break;
      case 'خط': case 'line':
        ctx.moveTo(args[0], args[1]);
        ctx.lineTo(args[2], args[3]);
        ctx.stroke();
        break;
      case 'نقطة': case 'point':
        ctx.fillRect(args[0], args[1], 2, 2);
        break;
      case 'مثلث': case 'triangle':
        ctx.moveTo(args[0], args[1]);
        ctx.lineTo(args[2], args[3]);
        ctx.lineTo(args[4], args[5]);
        ctx.closePath();
        ctx.stroke();
        break;
    }
  }

  // ملء("shape", args...)
  function fill(shape, args) {
    ensureCanvas();
    ctx.beginPath();
    switch (shape) {
      case 'مستطيل': case 'rect':
        ctx.fillRect(args[0], args[1], args[2], args[3]);
        break;
      case 'دائرة': case 'circle':
        ctx.arc(args[0], args[1], args[2], 0, Math.PI * 2);
        ctx.fill();
        break;
      case 'مثلث': case 'triangle':
        ctx.moveTo(args[0], args[1]);
        ctx.lineTo(args[2], args[3]);
        ctx.lineTo(args[4], args[5]);
        ctx.closePath();
        ctx.fill();
        break;
    }
  }

  // مستطيل(x, y, w, h)
  function rectangle(x, y, w, h) {
    ensureCanvas();
    ctx.strokeRect(x, y, w, h);
  }

  // دائرة(x, y, r)
  function circle(x, y, r) {
    ensureCanvas();
    ctx.beginPath();
    ctx.arc(x, y, r, 0, Math.PI * 2);
    ctx.stroke();
  }

  // خط(x1, y1, x2, y2)
  function line(x1, y1, x2, y2) {
    ensureCanvas();
    ctx.beginPath();
    ctx.moveTo(x1, y1);
    ctx.lineTo(x2, y2);
    ctx.stroke();
  }

  // نص_على_لوحة("text", x, y, fontSize)
  function textOnCanvas(text, x, y, fontSize) {
    ensureCanvas();
    ctx.font = (fontSize || 16) + 'px Arial';
    ctx.fillStyle = '#FFFFFF';
    ctx.fillText(text, x, y);
  }

  // مسح()
  function clear() {
    if (canvas && ctx) {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
    }
  }

  // ── Color Helpers ───────────────────────────────────────────────────────────
  function setColor(color) {
    ensureCanvas();
    ctx.strokeStyle = color;
    ctx.fillStyle = color;
  }

  function setLineWidth(width) {
    ensureCanvas();
    ctx.lineWidth = width;
  }

  // ── Canvas Info ─────────────────────────────────────────────────────────────
  function getCanvasSize() {
    ensureCanvas();
    return { width: canvas.width, height: canvas.height };
  }

  function setCanvasSize(w, h) {
    ensureCanvas(w, h);
    canvas.width = w;
    canvas.height = h;
  }

  // ── Public API ──────────────────────────────────────────────────────────────
  return {
    loadImage: loadImage,
    drawImage: drawImage,
    imageSize: imageSize,
    saveImage: saveImage,
    cropImage: cropImage,
    resize: resize,
    rotateImage: rotateImage,
    flipImage: flipImage,
    opacity: opacity,
    filter: filter,
    overlay: overlay,
    background: background,
    pixel: pixel,
    draw: draw,
    fill: fill,
    rectangle: rectangle,
    circle: circle,
    line: line,
    textOnCanvas: textOnCanvas,
    clear: clear,
    setColor: setColor,
    setLineWidth: setLineWidth,
    getCanvasSize: getCanvasSize,
    setCanvasSize: setCanvasSize,
    ensureCanvas: ensureCanvas,
    SUPPORTED_FORMATS: SUPPORTED_FORMATS,
    isFormatSupported: isFormatSupported,
    getSupportedFormats: getSupportedFormats,
    detectFormat: detectFormat
  };
})();

// Make __dhad available globally for codegen output
if (typeof window !== 'undefined') {
  window.__dhad = DhadImages;
} else if (typeof global !== 'undefined') {
  global.__dhad = DhadImages;
}

if (typeof module !== 'undefined' && module.exports) {
  module.exports = DhadImages;
}
