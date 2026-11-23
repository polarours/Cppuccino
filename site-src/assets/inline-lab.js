(function () {
  'use strict';

  function mount() {
    var host = document.querySelectorAll('[data-inline-lab]');
    Array.prototype.forEach.call(host, function (placeholder) {
      if (placeholder.dataset.mounted === 'true') return;
      placeholder.dataset.mounted = 'true';

      var lang = placeholder.dataset.lang ||
                 document.documentElement.dataset.lang || 'en';
      var theme = document.documentElement.dataset.readTheme || 'cappuccino';
      var base = placeholder.dataset.src || '../../lab/';
      var separator = base.indexOf('?') >= 0 ? '&' : '?';

      var frame = document.createElement('iframe');
      frame.src = base + separator + 'embed=1&lang=' +
                  encodeURIComponent(lang) + '&theme=' + encodeURIComponent(theme);
      frame.title = placeholder.dataset.label || 'Interactive lab';
      frame.loading = 'lazy';
      frame.className = 'inline-lab-frame';
      frame.dataset.labBase = base;

      var wrap = document.createElement('div');
      wrap.className = 'inline-lab-wrap';

      var head = document.createElement('div');
      head.className = 'inline-lab-head';
      head.textContent = placeholder.dataset.label || 'INTERACTIVE LAB';

      wrap.appendChild(head);
      wrap.appendChild(frame);
      placeholder.appendChild(wrap);
    });
  }

  function syncFrames() {
    var frames = document.querySelectorAll('iframe.inline-lab-frame');
    Array.prototype.forEach.call(frames, function (frame) {
      var lang = document.documentElement.dataset.lang || 'en';
      var theme = document.documentElement.dataset.readTheme || 'cappuccino';
      var base = frame.dataset.labBase || '../../lab/';
      var separator = base.indexOf('?') >= 0 ? '&' : '?';
      frame.src = base + separator + 'embed=1&lang=' +
                  encodeURIComponent(lang) + '&theme=' + encodeURIComponent(theme);
    });
  }

  mount();
  if (window.document$) window.document$.subscribe(mount);

  if (window.MutationObserver) {
    new MutationObserver(syncFrames).observe(document.documentElement, {
      attributes: true,
      attributeFilter: ['data-read-theme', 'data-lang']
    });
  }
})();
