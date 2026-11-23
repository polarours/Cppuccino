(function () {
  'use strict';

  var KEY = 'cppuccino-read-theme';
  var THEMES = [
    ['cappuccino', 'CAPPUCCINO'],
    ['arcade', 'ARCADE'],
    ['retro', 'RETRO']
  ];

  var current = 'cappuccino';
  try {
    var saved = localStorage.getItem(KEY);
    if (THEMES.some(function (entry) { return entry[0] === saved; })) {
      current = saved;
    }
  } catch (_) {}

  function apply() {
    document.documentElement.dataset.readTheme = current;
    try { localStorage.setItem(KEY, current); } catch (_) {}
    var buttons = document.querySelectorAll('[data-read-theme-option]');
    Array.prototype.forEach.call(buttons, function (button) {
      button.setAttribute('aria-pressed',
        button.dataset.readThemeOption === current ? 'true' : 'false');
    });
  }

  function init() {
    if (document.querySelector('.read-theme-switch')) {
      apply();
      return;
    }
    var header = document.querySelector('.md-header__inner') ||
                 document.querySelector('.md-header');
    if (!header) return;

    var group = document.createElement('div');
    group.className = 'read-theme-switch';
    group.setAttribute('role', 'group');
    group.setAttribute('aria-label', 'Reading theme');

    THEMES.forEach(function (entry) {
      var button = document.createElement('button');
      button.type = 'button';
      button.dataset.readThemeOption = entry[0];
      button.textContent = entry[1];
      button.title = entry[1] + ' theme';
      button.addEventListener('click', function () {
        current = entry[0];
        apply();
      });
      group.appendChild(button);
    });

    header.appendChild(group);
    apply();
  }

  init();
  if (window.document$) window.document$.subscribe(init);
})();
