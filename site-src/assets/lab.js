(function () {
  'use strict';

  var THEME_KEY = 'cppuccino-read-theme';
  var LANG_KEY = 'cppuccino-lab-lang';

  var TEXT = {
    en: {
      eyebrow: 'INTERACTIVE / 01 · OBJECT LIFETIME',
      title: 'LIFETIME TIMELINE',
      sub: 'Step through the code. Watch the object die — and see exactly when a reference becomes a ghost.',
      safe: 'SAFE LIFETIME',
      dangling: 'DANGLING REFERENCE',
      codeHead: 'SOURCE · ONE LINE AT A TIME',
      stateHead: 'RUNTIME STATE',
      owner: 'owner (unique_ptr)',
      ref: 'r (reference)',
      notCreated: 'NOT CREATED',
      unbound: 'UNBOUND',
      alive: 'ALIVE',
      dead: 'DESTROYED',
      valid: 'VALID',
      danglingState: 'DANGLING',
      heap: 'heap object',
      points: 'points at a live object',
      ghost: 'points at a dead object',
      start: 'Press STEP → to execute the first line.',
      created: 'owner created. The Resource lives on the heap.',
      bound: 'r is bound to the live object.',
      usedSafe: 'use(r) is safe: the owner is still alive.',
      resetSafe: 'owner.reset() destroys the object. Nothing uses r afterwards.',
      resetBad: 'owner.reset() destroys the object, but r still refers to it.',
      usedBad: 'UB: reading a destroyed object. This is heap-use-after-free.',
      doneSafe: 'Program ends cleanly. No undefined behavior.',
      doneBad: 'The program already invoked undefined behavior.',
      step: 'STEP →',
      reset: 'RESET ↺',
      stepCounter: 'STEP',
      article: 'REAL PITFALL ↗',
      boss: 'BOSS FIGHT ↗',
      footer: 'Every state here follows real C++ lifetime rules. No account, no score, no external service.',
      asanNote: 'Captured from a local -fsanitize=address repro; addresses omitted.'
    },
    zh: {
      eyebrow: '交互实验 / 01 · 对象生命周期',
      title: '生命周期时间线',
      sub: '逐行执行代码，看对象何时死亡，以及引用从哪一刻起变成幽灵。',
      safe: '安全生命周期',
      dangling: '悬垂引用',
      codeHead: '源码 · 一次一行',
      stateHead: '运行时状态',
      owner: 'owner（unique_ptr）',
      ref: 'r（引用）',
      notCreated: '尚未创建',
      unbound: '尚未绑定',
      alive: '存活',
      dead: '已销毁',
      valid: '有效',
      danglingState: '悬垂',
      heap: '堆对象',
      points: '指向活对象',
      ghost: '指向已死对象',
      start: '按 STEP → 执行第一行。',
      created: 'owner 创建，Resource 位于堆上。',
      bound: 'r 绑定到这个活对象。',
      usedSafe: 'use(r) 安全：owner 仍然活着。',
      resetSafe: 'owner.reset() 销毁对象，之后再也没有人使用 r。',
      resetBad: 'owner.reset() 销毁了对象，但 r 仍然引用它。',
      usedBad: 'UB：读取已销毁的对象，这就是 heap-use-after-free。',
      doneSafe: '程序正常结束，没有未定义行为。',
      doneBad: '程序已经触发了未定义行为。',
      step: '单步 →',
      reset: '重置 ↺',
      stepCounter: '第',
      article: '真实陷阱文章 ↗',
      boss: 'Boss 战 ↗',
      footer: '这里每个状态都遵循真实的 C++ 生命周期规则。无账号、无分数、无外部服务。',
      asanNote: '来自本地 -fsanitize=address 实测；地址已省略。'
    }
  };

  var SCENARIOS = {
    safe: [
      'auto owner = make_unique<Resource>();',
      'auto& r = *owner;',
      'use(r);',
      'owner.reset();',
      'return 0;'
    ],
    dangling: [
      'auto owner = make_unique<Resource>();',
      'auto& r = *owner;',
      'owner.reset();',
      'use(r);',
      'return 0;'
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

  var $ = function (id) { return document.getElementById(id); };
  var t = function () { return TEXT[state.lang]; };

  function save() {
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
    $('scenarioDangling').textContent = t().dangling;
    $('codeHead').textContent = t().codeHead;
    $('stateHead').textContent = t().stateHead;
    $('objectLabel').textContent = t().owner;
    $('refLabel').textContent = t().ref;
    $('stepBtn').textContent = t().step;
    $('resetBtn').textContent = t().reset;
    $('articleLink').textContent = t().article;
    $('bossLink').textContent = t().boss;
    $('footerNote').textContent = t().footer;
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('asan').textContent =
      '==ERROR: AddressSanitizer: heap-use-after-free\n' +
      'READ of size 4 ... thread T0\n' +
      '#0 ... in main ... dangling.cpp:7\n\n' + t().asanNote;
    Array.prototype.forEach.call(document.querySelectorAll('[data-lab-theme]'), function (button) {
      button.setAttribute('aria-pressed',
        button.dataset.labTheme === state.theme ? 'true' : 'false');
    });
    $('scenarioSafe').classList.toggle('active', state.scenario === 'safe');
    $('scenarioDangling').classList.toggle('active', state.scenario === 'dangling');
  }

  function simulate() {
    var alive = false;
    var bound = false;
    var key = 'start';
    var bad = false;
    var lines = SCENARIOS[state.scenario];

    for (var i = 0; i < state.step; i += 1) {
      var line = lines[i];
      if (/make_unique/.test(line)) {
        alive = true;
        key = 'created';
      } else if (/auto\s*&\s*\w+\s*=\s*\*\s*owner/.test(line)) {
        bound = true;
        key = 'bound';
      } else if (/owner\s*\.\s*reset\s*\(\s*\)|owner\s*=\s*nullptr/.test(line)) {
        alive = false;
        key = state.scenario === 'safe' ? 'resetSafe' : 'resetBad';
      } else if (/use\s*\(\s*\w+\s*\)/.test(line)) {
        if (alive) {
          key = 'usedSafe';
        } else {
          key = 'usedBad';
          bad = true;
        }
      } else if (/return\s+0\s*;/.test(line)) {
        key = bad ? 'doneBad' : (state.scenario === 'safe' ? 'doneSafe' : 'resetBad');
      }
    }

    return { alive: alive, bound: bound, key: key, bad: bad };
  }

  function render() {
    var lines = SCENARIOS[state.scenario];
    var result = simulate();
    var codeHost = $('codeLines');
    codeHost.innerHTML = '';

    lines.forEach(function (line, index) {
      var li = document.createElement('li');
      if (index + 1 < state.step) li.className = 'done';
      if (index + 1 === state.step) li.className = result.bad ? 'current bad' : 'current';
      li.innerHTML = '<span class="num">' + (index + 1) + '</span><span>' +
        line.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;') + '</span>';
      codeHost.appendChild(li);
    });

    $('stepCounter').textContent = t().stepCounter + ' ' + state.step + ' / ' + lines.length;

    $('objectCard').className = 'object-card ' + (result.alive ? 'alive' : (state.step ? 'dead' : ''));
    $('objectState').textContent = state.step === 0 ? t().notCreated : (result.alive ? t().alive : t().dead);
    $('objectNote').textContent = state.step === 0 ? '' : t().heap;

    var dangling = result.bound && !result.alive;
    $('refCard').className = 'object-card ' + (dangling ? 'dangling' : (result.bound && result.alive ? 'valid' : ''));
    $('refState').textContent = !result.bound ? t().unbound : (dangling ? t().danglingState : t().valid);
    $('refNote').textContent = !result.bound ? '' : (dangling ? t().ghost : t().points);
    $('arrow').className = 'arrow ' + (dangling ? 'dangling' : (result.bound ? 'valid' : ''));

    $('message').textContent = t()[result.key];
    $('message').className = 'message' + (result.bad ? ' bad' : '');
    $('asan').hidden = !(result.bad || (state.scenario === 'dangling' && state.step >= 4));
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
  $('scenarioDangling').addEventListener('click', function () { state.scenario = 'dangling'; reset(); applyChrome(); });
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
