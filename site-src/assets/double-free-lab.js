(function () {
  'use strict';

  var THEME_KEY = 'cppuccino-read-theme';
  var LANG_KEY = 'cppuccino-lab-lang';

  var TEXT = {
    en: {
      eyebrow: 'INTERACTIVE / 04 · OWNERSHIP',
      title: 'DOUBLE FREE',
      sub: 'One allocation, one owner. Step through the second delete that corrupts the allocator.',
      safe: 'SINGLE OWNER',
      bad: 'DOUBLE FREE',
      codeHead: 'SOURCE · ONE LINE AT A TIME',
      stateHead: 'RUNTIME STATE',
      pointer: 'p (pointer)',
      resource: 'Resource',
      unbound: 'UNBOUND',
      owns: 'OWNS',
      dangling: 'DANGLING',
      null: 'NULL',
      notAllocated: 'NOT ALLOCATED',
      alive: 'ALIVE',
      freed: 'FREED',
      heap: 'one heap allocation',
      pointsAlive: 'points at the live Resource',
      pointsFreed: 'still points at the freed Resource',
      nullNote: 'points at nothing; delete p is a no-op',
      start: 'Press STEP → to execute the first line.',
      created: 'new Resource() allocates one object and gives ownership to p.',
      used: 'use(p) is safe: the Resource is still alive.',
      released: 'delete p releases the object once and ends its lifetime.',
      nulled: 'p = nullptr. A later delete p is a safe no-op.',
      double: 'UB: the same object is deleted twice. ASan aborts.',
      doneSafe: 'Program ends cleanly. Exactly one owner released the object.',
      doneBad: 'The allocator was corrupted by the second delete.',
      step: 'STEP →',
      reset: 'RESET ↺',
      stepCounter: 'STEP',
      article: 'REAL PITFALL ↗',
      boss: 'BOSS FIGHT ↗',
      footer: 'Every state follows real allocation and release rules. No account, no score, no external service.',
      asanNote: 'Captured from a local -fsanitize=address repro; addresses omitted.'
    },
    zh: {
      eyebrow: '交互实验 / 04 · 所有权',
      title: '重复释放',
      sub: '一次分配，一个所有者。逐行看清第二次 delete 如何破坏分配器。',
      safe: '单一所有者',
      bad: '重复释放',
      codeHead: '源码 · 一次一行',
      stateHead: '运行时状态',
      pointer: 'p（指针）',
      resource: 'Resource',
      unbound: '尚未绑定',
      owns: '持有',
      dangling: '悬垂',
      null: '空指针',
      notAllocated: '尚未分配',
      alive: '存活',
      freed: '已释放',
      heap: '一次堆分配',
      pointsAlive: '指向存活的 Resource',
      pointsFreed: '仍指向已释放的 Resource',
      nullNote: '不指向任何对象；delete p 是空操作',
      start: '按 STEP → 执行第一行。',
      created: 'new Resource() 分配一个对象，并把所有权交给 p。',
      used: 'use(p) 安全：Resource 仍然存活。',
      released: 'delete p 释放对象一次，对象生命周期结束。',
      nulled: 'p = nullptr。之后再 delete p 是安全的空操作。',
      double: 'UB：同一个对象被 delete 两次，ASan 直接中止。',
      doneSafe: '程序正常结束。唯一的所有者只释放了一次。',
      doneBad: '第二次 delete 破坏了分配器。',
      step: '单步 →',
      reset: '重置 ↺',
      stepCounter: '第',
      article: '真实陷阱文章 ↗',
      boss: 'Boss 战 ↗',
      footer: '这里每个状态都遵循真实的分配与释放规则。无账号、无分数、无外部服务。',
      asanNote: '来自本地 -fsanitize=address 实测；地址已省略。'
    }
  };

  var SCENARIOS = {
    safe: [
      'auto p = new Resource();',
      'use(p);',
      'delete p;',
      'p = nullptr;'
    ],
    bad: [
      'auto p = new Resource();',
      'delete p;',
      'delete p;'
    ]
  };

  var state = { scenario: 'safe', step: 0, lang: 'en', theme: 'arcade' };
  try {
    state.lang = localStorage.getItem(LANG_KEY) === 'zh' ? 'zh' : 'en';
    var savedTheme = localStorage.getItem(THEME_KEY);
    if (['cappuccino', 'arcade', 'retro'].indexOf(savedTheme) >= 0) {
      state.theme = savedTheme;
    }
  } catch (_) {}

  var params = new URLSearchParams(location.search);
  var embedded = params.get('embed') === '1';
  if (embedded) document.body.classList.add('embed');
  if (params.get('lang') === 'zh' || params.get('lang') === 'en') state.lang = params.get('lang');
  if (['cappuccino', 'arcade', 'retro'].indexOf(params.get('theme')) >= 0) state.theme = params.get('theme');

  function $(id) { return document.getElementById(id); }
  function t() { return TEXT[state.lang]; }

  function save() {
    if (embedded) return;
    try {
      localStorage.setItem(LANG_KEY, state.lang);
      localStorage.setItem(THEME_KEY, state.theme);
    } catch (_) {}
  }

  function applyChrome() {
    document.documentElement.dataset.readTheme = state.theme;
    document.documentElement.dataset.lang = state.lang;
    document.documentElement.setAttribute(
      'lang', state.lang === 'zh' ? 'zh-CN' : 'en');
    $('labEyebrow').textContent = t().eyebrow;
    $('labTitle').textContent = t().title;
    $('labSub').textContent = t().sub;
    $('scenarioSafe').textContent = t().safe;
    $('scenarioBad').textContent = t().bad;
    $('codeHead').textContent = t().codeHead;
    $('stateHead').textContent = t().stateHead;
    $('objectLabel').textContent = t().pointer;
    $('refLabel').textContent = t().resource;
    $('stepBtn').textContent = t().step;
    $('resetBtn').textContent = t().reset;
    $('articleLink').textContent = t().article;
    $('bossLink').textContent = t().boss;
    $('footerNote').textContent = t().footer;
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('debugOut').textContent =
      '==ERROR: AddressSanitizer: attempting double-free on ... in thread T0:\n' +
      '#1 ... in main ... double.cpp:6\n\n' + t().asanNote;
    Array.prototype.forEach.call(document.querySelectorAll('[data-lab-theme]'), function (button) {
      button.setAttribute('aria-pressed', button.dataset.labTheme === state.theme ? 'true' : 'false');
    });
    $('scenarioSafe').classList.toggle('active', state.scenario === 'safe');
    $('scenarioBad').classList.toggle('active', state.scenario === 'bad');
  }

  function simulate() {
    var allocated = false;
    var alive = false;
    var isNull = false;
    var bad = false;
    var key = 'start';
    var lines = SCENARIOS[state.scenario];

    for (var i = 0; i < state.step; i += 1) {
      var line = lines[i];
      if (/new\s+Resource/.test(line)) {
        allocated = true;
        alive = true;
        key = 'created';
      } else if (/use\s*\(\s*p\s*\)/.test(line)) {
        key = alive ? 'used' : 'double';
      } else if (/delete\s+p/.test(line)) {
        if (alive) {
          alive = false;
          key = 'released';
        } else {
          bad = true;
          key = 'double';
        }
      } else if (/p\s*=\s*nullptr/.test(line)) {
        isNull = true;
        key = 'nulled';
      }
    }

    if (state.step === lines.length) {
      key = bad ? 'doneBad' : 'doneSafe';
    }

    return {
      allocated: allocated, alive: alive, isNull: isNull, bad: bad, key: key,
      lines: lines
    };
  }

  function render() {
    var result = simulate();
    var codeHost = $('codeLines');
    codeHost.innerHTML = '';

    result.lines.forEach(function (line, index) {
      var li = document.createElement('li');
      if (index + 1 < state.step) li.className = 'done';
      if (index + 1 === state.step) li.className = result.bad ? 'current bad' : 'current';
      li.innerHTML = '<span class="num">' + (index + 1) + '</span><span>' +
        line.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</span>';
      codeHost.appendChild(li);
    });

    $('stepCounter').textContent = t().stepCounter + ' ' + state.step + ' / ' + result.lines.length;

    var pointerState = !result.allocated ? t().unbound :
      (result.isNull ? t().null : (result.alive ? t().owns : t().dangling));
    var pointerClass = !result.allocated ? '' :
      (result.isNull ? 'dead' : (result.alive ? 'valid' : 'dangling'));
    $('objectCard').className = 'object-card ' + pointerClass;
    $('objectState').textContent = pointerState;
    $('objectNote').textContent = !result.allocated ? '' :
      (result.isNull ? t().nullNote : (result.alive ? t().pointsAlive : t().pointsFreed));

    $('refCard').className = 'object-card ' +
      (!result.allocated ? '' : (result.alive ? 'alive' : 'dangling'));
    $('refState').textContent = !result.allocated ? t().notAllocated :
      (result.alive ? t().alive : t().freed);
    $('refNote').textContent = !result.allocated ? '' : t().heap;

    var dangling = result.allocated && !result.alive && !result.isNull;
    $('arrow').className = 'arrow ' + (dangling ? 'dangling' : (result.alive ? 'valid' : ''));

    $('message').textContent = t()[result.key];
    $('message').className = 'message' + (result.bad ? ' bad' : '');
    $('debugOut').hidden = !result.bad;
  }

  function reset() { state.step = 0; render(); }
  function step() {
    var total = SCENARIOS[state.scenario].length;
    if (state.step < total) state.step += 1;
    render();
  }

  $('stepBtn').addEventListener('click', step);
  $('resetBtn').addEventListener('click', reset);
  $('scenarioSafe').addEventListener('click', function () { state.scenario = 'safe'; reset(); applyChrome(); });
  $('scenarioBad').addEventListener('click', function () { state.scenario = 'bad'; reset(); applyChrome(); });
  $('langBtn').addEventListener('click', function () {
    state.lang = state.lang === 'en' ? 'zh' : 'en';
    save(); applyChrome(); render();
  });

  Array.prototype.forEach.call(document.querySelectorAll('[data-lab-theme]'), function (button) {
    button.addEventListener('click', function () {
      state.theme = button.dataset.labTheme;
      save(); applyChrome();
    });
  });

  document.addEventListener('keydown', function (event) {
    if (event.key === 'ArrowRight' || event.key === ' ') { event.preventDefault(); step(); }
    if (event.key.toLowerCase() === 'r') reset();
  });

  applyChrome();
  render();
})();
