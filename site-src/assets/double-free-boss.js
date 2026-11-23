(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const QUEST_ID = 'challenge-en-boss-double-free';

  const TEXT = {
    en: {
      stage: 'STAGE 03 · BOSS',
      name: 'DOUBLE FREE',
      sub: 'It convinces you that releasing the same object twice is harmless.',
      hero: 'YOU',
      intro: 'Write a spell that uses the resource while it lives, then releases it exactly once.',
      hint: 'Allowed spell: new, use(p), delete p, p = nullptr. Three safe uses defeat the boss; a second delete on a live pointer is UB.',
      run: 'RUN ▶',
      retry: 'RETRY ↺',
      cleared: 'CLEARED ✓',
      debug: 'Q · DEBUG',
      debugHead: 'DEBUGGER · SANITIZER REPORT',
      allowed: 'ALLOWED SPELLS:',
      hit: 'HIT! The object is still alive. Boss HP -34.',
      double: 'MISS! You deleted the same object twice.',
      uaf: 'MISS! You used an object after delete.',
      unbound: 'NO POINTER YET. Create it with new Resource().',
      win: 'STAGE CLEARED. Exactly one owner released the resource.',
      lose: 'GAME OVER. Ownership is a mess. Retry.',
      reportNote: 'Captured from a local -fsanitize=address repro; addresses omitted.',
      footer: 'The browser runs a small teaching DSL. It cannot compile arbitrary C++; the links above open the real pitfall and example.',
      article: 'READ THE REAL PITFALL ↗',
      example: 'RUNNABLE EXAMPLE ↗',
      created: 'p created: one owner, one heap object.',
      used: 'use(p) while alive — safe.',
      released: 'delete p released the object once.',
      nulled: 'p = nullptr; deleting nullptr is a no-op.'
    },
    zh: {
      stage: '第 03 关 · BOSS',
      name: '重复释放',
      sub: '它让你相信，把同一个对象释放两次没什么大不了。',
      hero: '你',
      intro: '写一段咒语：资源活着时使用它，然后只释放一次。',
      hint: '可用咒语：new、use(p)、delete p、p = nullptr。三次安全使用即可击败 Boss；对活指针二次 delete 是 UB。',
      run: '运行 ▶',
      retry: '重试 ↺',
      cleared: '已通关 ✓',
      debug: 'Q · 调试',
      debugHead: '调试器 · Sanitizer 报告',
      allowed: '可用咒语：',
      hit: '命中！对象仍然存活，Boss 生命 -34。',
      double: '失手！你把同一个对象释放了两次。',
      uaf: '失手！你在 delete 之后使用了对象。',
      unbound: '还没有指针。先用 new Resource() 创建。',
      win: '通关。资源被唯一的所有者释放了一次。',
      lose: '游戏结束。所有权已经乱了，重试。',
      reportNote: '来自本地 -fsanitize=address 实测；地址已省略。',
      footer: '浏览器里运行的是教学 DSL，不能编译任意 C++；上面链接打开真实陷阱文章和示例。',
      article: '阅读真实陷阱文章 ↗',
      example: '可运行示例 ↗',
      created: 'p 已创建：一个所有者，一个堆对象。',
      used: '对象存活期间 use(p)——安全。',
      released: 'delete p 释放对象一次。',
      nulled: 'p = nullptr；delete nullptr 是空操作。'
    }
  };

  const defaults = { theme: 'arcade', lang: 'en', cleared: [] };
  let state = Object.assign({}, defaults);
  try {
    const saved = JSON.parse(localStorage.getItem(STORE) || '{}');
    state = Object.assign({}, defaults, saved);
    if (!Array.isArray(state.cleared)) state.cleared = [];
  } catch (_) {}

  const $ = (id) => document.getElementById(id);
  const t = () => TEXT[state.lang];
  let heroHp = 100;
  let bossHp = 100;
  let won = state.cleared.indexOf(QUEST_ID) >= 0;
  let report = '';

  function save() {
    try { localStorage.setItem(STORE, JSON.stringify(state)); } catch (_) {}
  }

  function applyChrome() {
    document.documentElement.dataset.theme = state.theme;
    document.documentElement.dataset.lang = state.lang;
    $('themeBtn').textContent = state.theme === 'arcade' ? 'RETRO' : 'ARCADE';
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('stageCode').textContent = t().stage;
    $('bossName').textContent = t().name;
    $('bossSub').textContent = t().sub;
    $('bossLabel').textContent = t().name;
    $('heroLabel').textContent = t().hero;
    $('dialogue').textContent = t().intro;
    $('editorHint').textContent = t().hint;
    $('debugBtn').textContent = t().debug;
    $('runBtn').textContent = won ? t().cleared : (heroHp <= 0 ? t().retry : t().run);
    $('debuggerHead').textContent = t().debugHead;
    renderReport();
    $('allowed').innerHTML = t().allowed + ' ' +
      '<code>new Resource()</code> · ' +
      '<code>use(p)</code> · ' +
      '<code>delete p</code> · ' +
      '<code>p = nullptr</code> · ' +
      '<code>return 0;</code>';
    $('footerNote').textContent = t().footer;
    $('articleLink').textContent = t().article;
    $('exampleLink').textContent = t().example;
    updateHp();
  }

  function renderReport() {
    const first = report === 'double'
      ? '==ERROR: AddressSanitizer: attempting double-free on ... in thread T0:\n' +
        '#1 ... in main ... double.cpp:6'
      : '==ERROR: AddressSanitizer: heap-use-after-free on ...\n' +
        'READ of size 4 ... thread T0';
    $('debuggerReport').textContent = first + '\n\n' + t().reportNote;
  }

  function updateHp() {
    $('heroHp').style.width = Math.max(0, heroHp) + '%';
    $('bossHp').style.width = Math.max(0, bossHp) + '%';
    $('heroHpText').textContent = Math.max(0, heroHp) + ' / 100';
    $('bossHpText').textContent = Math.max(0, bossHp) + ' / 100';
  }

  function log(message, className) {
    const line = document.createElement('span');
    line.className = className || '';
    line.textContent = '\n' + message;
    $('battleLog').appendChild(line);
    $('battleLog').scrollTop = $('battleLog').scrollHeight;
  }

  function glitch(message, kind) {
    $('arena').classList.remove('glitch');
    void $('arena').offsetWidth;
    $('arena').classList.add('glitch');
    heroHp -= 25;
    report = kind || 'double';
    renderReport();
    log(message, 'miss');
    $('dialogue').textContent = message;
    $('debugger').hidden = false;
    updateHp();
  }

  function resetBattle() {
    heroHp = 100;
    bossHp = 100;
    won = state.cleared.indexOf(QUEST_ID) >= 0;
    report = '';
    $('battleLog').textContent = '';
    $('debugger').hidden = true;
    $('dialogue').textContent = t().intro;
    $('runBtn').textContent = won ? t().cleared : t().run;
    updateHp();
  }

  function runSpell() {
    if (heroHp <= 0 || won) { resetBattle(); return; }

    let alive = false;
    let bound = false;
    let isNull = false;
    let damaged = 0;
    const lines = $('code').value.split('\n');

    for (const rawLine of lines) {
      const line = rawLine.split('//')[0].trim();
      if (!line) continue;

      if (/new\s+Resource\s*\(\s*\)/.test(line)) {
        alive = true;
        bound = true;
        isNull = false;
        log(t().created);
      } else if (/use\s*\(\s*p\s*\)/.test(line)) {
        if (!bound) { glitch(t().unbound, 'uaf'); return; }
        if (isNull) { glitch(t().uaf, 'uaf'); return; }
        if (!alive) { glitch(t().uaf, 'uaf'); return; }
        bossHp -= 34;
        damaged += 34;
        log(t().hit, 'hit');
      } else if (/delete\s+p\s*;/.test(line)) {
        if (isNull) {
          log(t().nulled);
        } else if (alive) {
          alive = false;
          log(t().released);
        } else {
          glitch(t().double, 'double');
          return;
        }
      } else if (/p\s*=\s*nullptr/.test(line)) {
        isNull = true;
        log(t().nulled);
      } else if (/return\s+0\s*;/.test(line)) {
        break;
      } else {
        glitch(t().double, 'double');
        return;
      }

      if (bossHp <= 0) break;
    }

    updateHp();
    if (bossHp <= 0) {
      won = true;
      if (state.cleared.indexOf(QUEST_ID) < 0) state.cleared.push(QUEST_ID);
      save();
      $('dialogue').textContent = t().win;
      $('runBtn').textContent = t().cleared;
      log(t().win, 'hit');
    } else if (damaged === 0 && heroHp === 100) {
      $('dialogue').textContent = t().hint;
    }
  }

  $('runBtn').addEventListener('click', runSpell);
  $('debugBtn').addEventListener('click', () => {
    $('debugger').hidden = !$('debugger').hidden;
  });
  $('closeDebug').addEventListener('click', () => { $('debugger').hidden = true; });

  $('themeBtn').addEventListener('click', () => {
    state.theme = state.theme === 'arcade' ? 'retro' : 'arcade';
    save(); applyChrome();
  });

  $('langBtn').addEventListener('click', () => {
    state.lang = state.lang === 'en' ? 'zh' : 'en';
    save(); applyChrome();
  });

  document.addEventListener('keydown', (event) => {
    const tag = document.activeElement && document.activeElement.tagName;
    if (tag === 'TEXTAREA' || tag === 'INPUT') return;
    if (event.key.toLowerCase() === 'q') {
      $('debugger').hidden = !$('debugger').hidden;
    } else if (event.key === 'Escape') {
      $('debugger').hidden = true;
    } else if (event.ctrlKey && event.key === 'Enter') {
      event.preventDefault();
      runSpell();
    }
  });

  applyChrome();
})();
