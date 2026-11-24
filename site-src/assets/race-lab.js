(function () {
  'use strict';

  var THEME_KEY = 'cppuccino-read-theme';
  var LANG_KEY = 'cppuccino-lab-lang';

  var TEXT = {
    en: {
      eyebrow: 'INTERACTIVE / 03 · CONCURRENCY',
      title: 'DATA RACE',
      sub: 'Two threads, one counter. Step through the interleaving that loses an update.',
      safe: 'MUTEX',
      bad: 'RACE',
      codeHead: 'SOURCE · ONE STEP AT A TIME',
      stateHead: 'THREAD STATE',
      shared: 'shared (int)',
      sync: 'guarded by m',
      unsync: 'no synchronization',
      idle: 'IDLE',
      waiting: 'WAITING',
      reading: 'READ {v}',
      wrote: 'WROTE {v}',
      locked: 'LOCKED',
      start: 'Press STEP → to execute the first line.',
      mutex: 'm is created. Both increments will use the same mutex.',
      zeroed: 'shared starts at 0.',
      acquired: 'The lock is acquired. The next increments are serialized.',
      bothRead: 'Both threads read 0 before either writes. TSan reports a data race.',
      t1Read: 'T1 reads 0, then T2 reads the same 0.',
      t1Write: 'T1 writes 1. T2 is still holding its stale read of 0.',
      t2Write: 'T2 writes 1. One increment is lost: shared is 1, expected 2.',
      safeIncrement: 'The mutex serializes the increment. shared is now {v}.',
      safeDone: 'Program ends cleanly. Both increments landed.',
      doneBad: 'The race already happened. shared is 1, expected 2.',
      step: 'STEP →',
      reset: 'RESET ↺',
      stepCounter: 'STEP',
      article: 'REAL PITFALL ↗',
      boss: 'BOSS FIGHT ↗',
      footer: 'Every state follows real interleaving rules. No account, no score, no external service.',
      tsanNote: 'Captured from a local -fsanitize=thread repro; addresses omitted.'
    },
    zh: {
      eyebrow: '交互实验 / 03 · 并发',
      title: '数据竞争',
      sub: '两个线程、一个计数器。逐帧看清丢失更新的交错顺序。',
      safe: '互斥锁',
      bad: '竞争',
      codeHead: '源码 · 一步一步',
      stateHead: '线程状态',
      shared: 'shared（int）',
      sync: '由 m 保护',
      unsync: '没有同步',
      idle: '空闲',
      waiting: '等待',
      reading: '读到 {v}',
      wrote: '写入 {v}',
      locked: '已加锁',
      start: '按 STEP → 执行第一行。',
      mutex: 'm 已创建。两次自增都会使用同一把互斥锁。',
      zeroed: 'shared 从 0 开始。',
      acquired: '锁已获取，接下来的自增被串行化。',
      bothRead: '两个线程都在对方写入前读到了 0。TSan 报 data race。',
      t1Read: 'T1 读到 0，随后 T2 也读到同一个 0。',
      t1Write: 'T1 写入 1。T2 手里还握着过期的 0。',
      t2Write: 'T2 写入 1。一次自增丢失：shared 是 1，期望是 2。',
      safeIncrement: '互斥锁把自增串行化。shared 现在是 {v}。',
      safeDone: '程序正常结束，两次自增都生效了。',
      doneBad: '竞争已经发生。shared 是 1，期望是 2。',
      step: '单步 →',
      reset: '重置 ↺',
      stepCounter: '第',
      article: '真实陷阱文章 ↗',
      boss: 'Boss 战 ↗',
      footer: '这里每个状态都遵循真实的交错规则。无账号、无分数、无外部服务。',
      tsanNote: '来自本地 -fsanitize=thread 实测；地址已省略。'
    }
  };

  var SCENARIOS = {
    safe: [
      'std::mutex m;',
      'int shared = 0;',
      'std::lock_guard<std::mutex> g(m);',
      '++shared;',
      '++shared;'
    ],
    bad: [
      'int shared = 0;',
      'int a = shared;  // T1 reads',
      'int b = shared;  // T2 reads',
      'shared = a + 1;  // T1 writes',
      'shared = b + 1;  // T2 writes'
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
  function fill(template, value) { return template.replace('{v}', String(value)); }

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
    $('labEyebrow').textContent = t().eyebrow;
    $('labTitle').textContent = t().title;
    $('labSub').textContent = t().sub;
    $('scenarioSafe').textContent = t().safe;
    $('scenarioBad').textContent = t().bad;
    $('codeHead').textContent = t().codeHead;
    $('stateHead').textContent = t().stateHead;
    $('objectLabel').textContent = t().shared;
    $('stepBtn').textContent = t().step;
    $('resetBtn').textContent = t().reset;
    $('articleLink').textContent = t().article;
    $('bossLink').textContent = t().boss;
    $('footerNote').textContent = t().footer;
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('debugOut').textContent =
      'WARNING: ThreadSanitizer: data race\n' +
      'Read of size 4 ... by thread T2\n' +
      '#0 operator() ... race.cpp:6\n\n' + t().tsanNote;
    Array.prototype.forEach.call(document.querySelectorAll('[data-lab-theme]'), function (button) {
      button.setAttribute('aria-pressed', button.dataset.labTheme === state.theme ? 'true' : 'false');
    });
    $('scenarioSafe').classList.toggle('active', state.scenario === 'safe');
    $('scenarioBad').classList.toggle('active', state.scenario === 'bad');
  }

  function simulate() {
    var lines = SCENARIOS[state.scenario];
    var shared = 0;
    var t1 = 'idle';
    var t2 = 'idle';
    var t1Value = null;
    var t2Value = null;
    var race = false;
    var key = 'start';

    for (var i = 0; i < state.step; i += 1) {
      var line = lines[i];

      if (/^std::mutex\s+\w+\s*;/.test(line)) {
        key = 'mutex';
      } else if (/int\s+shared/.test(line)) {
        key = 'zeroed';
      } else if (/lock_guard/.test(line)) {
        key = 'acquired';
        t1 = 'locked';
      } else if (/int\s+a\s*=\s*shared/.test(line)) {
        t1 = 'read';
        t1Value = shared;
        key = 't1Read';
      } else if (/int\s+b\s*=\s*shared/.test(line)) {
        t2 = 'read';
        t2Value = shared;
        key = 'bothRead';
        race = true;
      } else if (/shared\s*=\s*a\s*\+\s*1/.test(line)) {
        shared = t1Value + 1;
        t1 = 'wrote';
        key = 't1Write';
      } else if (/shared\s*=\s*b\s*\+\s*1/.test(line)) {
        shared = t2Value + 1;
        t2 = 'wrote';
        key = 't2Write';
      } else if (/\+\+shared/.test(line)) {
        shared += 1;
        if (t1 === 'locked') {
          t1 = 'wrote';
          t1Value = shared;
        } else {
          t2 = 'wrote';
          t2Value = shared;
        }
        key = 'safeIncrement';
      }
    }

    return {
      shared: shared, t1: t1, t2: t2, t1Value: t1Value, t2Value: t2Value,
      race: race, key: key, lines: lines
    };
  }

  function renderThread(key, mode, value) {
    var card = $('thread' + key);
    var text = $('t' + key + 'State');
    var note = $('t' + key + 'Note');
    card.className = 'thread-card' + (mode === 'read' ? ' reading' : '') +
      (mode === 'wrote' && state.scenario === 'bad' && key === '2' ? ' bad' : '');
    if (mode === 'idle') {
      text.textContent = t().idle;
      note.textContent = '';
    } else if (mode === 'locked') {
      text.textContent = t().locked;
      note.textContent = '';
    } else if (mode === 'read') {
      text.textContent = fill(t().reading, value);
      note.textContent = '';
    } else {
      text.textContent = fill(t().wrote, value);
      note.textContent = '';
    }
  }

  function render() {
    var result = simulate();
    var codeHost = $('codeLines');
    codeHost.innerHTML = '';

    result.lines.forEach(function (line, index) {
      var li = document.createElement('li');
      if (index + 1 < state.step) li.className = 'done';
      if (index + 1 === state.step) li.className = result.race ? 'current bad' : 'current';
      li.innerHTML = '<span class="num">' + (index + 1) + '</span><span>' +
        line.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</span>';
      codeHost.appendChild(li);
    });

    $('stepCounter').textContent = t().stepCounter + ' ' + state.step + ' / ' + result.lines.length;
    $('objectState').textContent = String(result.shared);
    $('objectNote').textContent = state.scenario === 'safe' ? t().sync : t().unsync;
    $('objectCard').className = 'object-card ' + (result.race ? 'dangling' : 'valid');

    renderThread('1', result.t1, result.t1Value);
    renderThread('2', result.t2, result.t2Value);

    var message = result.key === 'safeIncrement'
      ? fill(t().safeIncrement, result.shared)
      : t()[result.key];
    if (state.step === result.lines.length) {
      message = result.race ? t().doneBad : t().safeDone;
    }
    $('message').textContent = message;
    $('message').className = 'message' + (result.race ? ' bad' : '');
    $('debugOut').hidden = !result.race;
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
