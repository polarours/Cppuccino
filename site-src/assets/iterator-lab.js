(function () {
  'use strict';

  var THEME_KEY = 'cppuccino-read-theme';
  var LANG_KEY = 'cppuccino-lab-lang';

  var TEXT = {
    en: {
      eyebrow: 'INTERACTIVE / 02 · CONTAINER STABILITY',
      title: 'ITERATOR INVALIDATION',
      sub: 'Grow a vector and watch the old iterator keep pointing at the buffer it just lost.',
      safe: 'SAFE REACQUIRE',
      bad: 'STALE ITERATOR',
      codeHead: 'SOURCE · ONE LINE AT A TIME',
      stateHead: 'RUNTIME STATE',
      vector: 'v (vector)',
      iterator: 'it (iterator)',
      empty: 'EMPTY',
      live: 'LIVE',
      unbound: 'UNBOUND',
      valid: 'VALID',
      stale: 'STALE',
      bufferA: 'buffer @ A · cap 3',
      bufferB: 'buffer @ B · cap 6',
      pointsA: 'points into buffer @ A',
      pointsB: 'points into buffer @ B',
      ghost: 'dangles at the freed buffer @ A',
      start: 'Press STEP → to execute the first line.',
      created: 'v owns a buffer of capacity 3 at address A.',
      bound: 'it points at the first element in buffer A.',
      moved: 'push_back reallocates: the buffer moves to B. it still points at A.',
      reacquired: 'it = v.begin() reacquires the iterator from the new buffer.',
      usedSafe: 'Dereferencing the reacquired iterator is safe.',
      usedBad: 'UB: the iterator points at the old, freed buffer. _GLIBCXX_DEBUG aborts.',
      doneSafe: 'Program ends cleanly. The iterator was reacquired after reallocation.',
      doneBad: 'Invalidation was already used. The debug iterator aborted the program.',
      step: 'STEP →',
      reset: 'RESET ↺',
      stepCounter: 'STEP',
      article: 'REAL PITFALL ↗',
      boss: 'BOSS FIGHT ↗',
      footer: 'Every state follows real vector reallocation rules. No account, no score, no external service.',
      debugNote: 'Captured from a local -D_GLIBCXX_DEBUG repro; paths trimmed.'
    },
    zh: {
      eyebrow: '交互实验 / 02 · 容器稳定性',
      title: '迭代器失效',
      sub: '让 vector 扩容，看旧迭代器如何继续指着已经失去的缓冲区。',
      safe: '安全重取',
      bad: '失效迭代器',
      codeHead: '源码 · 一次一行',
      stateHead: '运行时状态',
      vector: 'v（vector）',
      iterator: 'it（迭代器）',
      empty: '空',
      live: '存活',
      unbound: '尚未绑定',
      valid: '有效',
      stale: '已失效',
      bufferA: '缓冲区 @ A · 容量 3',
      bufferB: '缓冲区 @ B · 容量 6',
      pointsA: '指向缓冲区 A',
      pointsB: '指向缓冲区 B',
      ghost: '悬在已释放的缓冲区 A',
      start: '按 STEP → 执行第一行。',
      created: 'v 持有容量 3 的缓冲区，地址为 A。',
      bound: 'it 指向缓冲区 A 的第一个元素。',
      moved: 'push_back 触发扩容：缓冲区搬到 B，it 仍指向 A。',
      reacquired: 'it = v.begin() 从新缓冲区重新取得迭代器。',
      usedSafe: '解引用重新取得的迭代器是安全的。',
      usedBad: 'UB：迭代器指向旧的、已释放的缓冲区，_GLIBCXX_DEBUG 直接中止。',
      doneSafe: '程序正常结束。扩容后重新取得了迭代器。',
      doneBad: '失效迭代器已经被使用，调试迭代器中止了程序。',
      step: '单步 →',
      reset: '重置 ↺',
      stepCounter: '第',
      article: '真实陷阱文章 ↗',
      boss: 'Boss 战 ↗',
      footer: '这里每个状态都遵循真实的 vector 扩容规则。无账号、无分数、无外部服务。',
      debugNote: '来自本地 -D_GLIBCXX_DEBUG 实测；路径已裁剪。'
    }
  };

  var SCENARIOS = {
    safe: [
      'std::vector<int> v{1, 2, 3};',
      'auto it = v.begin();',
      'v.push_back(4);',
      'it = v.begin();',
      'use(*it);'
    ],
    bad: [
      'std::vector<int> v{1, 2, 3};',
      'auto it = v.begin();',
      'v.push_back(4);',
      'use(*it);'
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
    $('labEyebrow').textContent = t().eyebrow;
    $('labTitle').textContent = t().title;
    $('labSub').textContent = t().sub;
    $('scenarioSafe').textContent = t().safe;
    $('scenarioBad').textContent = t().bad;
    $('codeHead').textContent = t().codeHead;
    $('stateHead').textContent = t().stateHead;
    $('objectLabel').textContent = t().vector;
    $('refLabel').textContent = t().iterator;
    $('stepBtn').textContent = t().step;
    $('resetBtn').textContent = t().reset;
    $('articleLink').textContent = t().article;
    $('bossLink').textContent = t().boss;
    $('footerNote').textContent = t().footer;
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('debugOut').textContent =
      '/usr/include/c++/16/debug/safe_iterator.h:352:\n' +
      'Error: attempt to dereference a singular iterator.\n\n' +
      'Objects involved in the operation:\n' +
      '    iterator "this" ... state = singular;\n\n' + t().debugNote;
    Array.prototype.forEach.call(document.querySelectorAll('[data-lab-theme]'), function (button) {
      button.setAttribute('aria-pressed', button.dataset.labTheme === state.theme ? 'true' : 'false');
    });
    $('scenarioSafe').classList.toggle('active', state.scenario === 'safe');
    $('scenarioBad').classList.toggle('active', state.scenario === 'bad');
  }

  function simulate() {
    var created = false;
    var bound = false;
    var reallocated = false;
    var reacquired = false;
    var used = false;
    var bad = false;
    var key = 'start';
    var lines = SCENARIOS[state.scenario];

    for (var i = 0; i < state.step; i += 1) {
      var line = lines[i];
      if (/std::vector/.test(line)) {
        created = true;
        key = 'created';
      } else if (/(?:auto\s+)?it\s*=\s*v\s*\.\s*begin/.test(line)) {
        bound = true;
        key = reallocated ? 'reacquired' : 'bound';
        if (reallocated) reacquired = true;
      } else if (/push_back/.test(line)) {
        reallocated = true;
        reacquired = false;
        key = 'moved';
      } else if (/use\s*\(\s*\*?\s*it\s*\)/.test(line)) {
        used = true;
        if (reallocated && !reacquired) {
          bad = true;
          key = 'usedBad';
        } else {
          key = 'usedSafe';
        }
      }
    }

    if (key === 'start' && state.step > 0) {
      key = bad ? 'doneBad' : (state.scenario === 'safe' ? 'doneSafe' : 'usedBad');
    }

    return {
      created: created,
      bound: bound,
      reallocated: reallocated,
      reacquired: reacquired,
      used: used,
      bad: bad,
      key: key
    };
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

    $('objectCard').className = 'object-card ' + (result.created ? 'alive' : (state.step ? 'dead' : ''));
    $('objectState').textContent = state.step === 0 ? t().empty : t().live;
    $('objectNote').textContent = state.step === 0 ? '' :
      (result.reallocated ? t().bufferB : t().bufferA);

    var stale = result.bound && result.reallocated && !result.reacquired;
    $('refCard').className = 'object-card ' +
      (stale ? 'dangling' : (result.bound ? 'valid' : ''));
    $('refState').textContent = !result.bound ? t().unbound : (stale ? t().stale : t().valid);
    $('refNote').textContent = !result.bound ? '' :
      (stale ? t().ghost : (result.reacquired ? t().pointsB : t().pointsA));
    $('arrow').className = 'arrow ' + (stale ? 'dangling' : (result.bound ? 'valid' : ''));

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
