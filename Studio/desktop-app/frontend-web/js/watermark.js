(function(){
  var text = 'تم برمجة المشروع من قبل محمود الحموز';
  var existing = document.querySelector('.dhad-watermark');
  if (existing) return;
  var container = document.createElement('div');
  container.className = 'dhad-watermark';
  container.setAttribute('aria-hidden', 'true');
  var rows = 6;
  var cols = 4;
  for (var i = 0; i < rows * cols; i++) {
    var span = document.createElement('span');
    span.textContent = text;
    container.appendChild(span);
  }
  if (document.body) {
    document.body.appendChild(container);
  } else {
    document.addEventListener('DOMContentLoaded', function() {
      document.body.appendChild(container);
    });
  }
})();
