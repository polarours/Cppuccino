(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const QUEST_ID = 'challenge-en-boss-dangling-pointer';

  const TEXT = {
    en: {
      stage: 'STAGE 01 · BOSS',
      name: 'DANGLING POINTER',
      sub: 'It steals your objects before you finish using them.',
      hero: 'YOU',
      intro: 'Write a spell that keeps the resource alive while you use it. Wrong lifetime, wrong ending.',
      hint: 'Allowed spell: create owner, bind a reference, use it, then release. Repeat use(r) to attack.',
      run: 'RUN ▶',
      retry: 'RETRY ↺',
      cleared: 'CLEARED ✓',
      debug: 'Q · DEBUG',
      debugHead: 'DEBUGGER · SANITIZER REPORT',
      allowed: 'ALLOWED SPELLS:',
      hit: 'HIT! The reference is still valid. Boss HP -34.',
      miss: 'MISS! You used a reference after its owner died.',
      unknown: 'UNKNOWN SPELL. Check the allowed list below.',
      syntax: 'NO REFERENCE YET. Bind r before using it.',
      win: 'STAGE CLEARED. The pointer no longer dangles.',
      lose: 'GAME OVER. Your references all dangle. Retry.',
      reportNote: 'Captured from a local -fsanitize=address repro; addresses omitted.',
      footer: 'The browser runs a small teaching DSL. It cannot compile arbitrary C++; the link above opens the real example.',
      article: 'READ THE REAL PITFALL ↗',
      example: 'RUNNABLE EXAMPLE ↗'
    },
    zh: {
      stage: '第 01 关 · BOSS',
      name: '悬垂引用',
      sub: '它会在你用完之前把对象偷走。',
      hero: '你',
      intro: '写一段咒语，让资源在你使用期间一直活着。生命周期错了，结局就错了。',
      hint: '可用咒语：创建 owner、绑定引用、使用、最后释放。多写几行 use(r) 可以连续攻击。',
      run: '运行 ▶',
      retry: '重试 ↺',
      cleared: '已通关 ✓',
      debug: 'Q · 调试',
      debugHead: '调试器 · Sanitizer 报告',
      allowed: '可用咒语：',
      hit: '命中！引用仍然有效，Boss 生命 -34。',
      miss: '失手！你在 owner 销毁之后使用了引用。',
      unknown: '未知咒语。请看下面的可用列表。',
      syntax: '还没有引用。先绑定 r 再使用。',
      win: '通关。引用不再悬垂。',
      lose: '游戏结束。你的引用全悬垂了，重试。',
      reportNote: '来自本地 -fsanitize=address 实测；地址已省略。',
      footer: '浏览器里运行的是教学 DSL，不能编译任意 C++；上面的链接打开真实示例。',
      article: '阅读真实陷阱文章 ↗',
      example: '可运行示例 ↗'
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

  function save() {
    try { localStorage.setItem(STORE, JSON.stringify(state)); } catch (_) {}
  }

  function applyChrome() {
    document.documentElement.dataset.theme = state.theme;
    document.documentElement.dataset.lang = state.lang;
    document.documentElement.setAttribute(
      'lang', state.lang === 'zh' ? 'zh-CN' : 'en');
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
    $('debuggerReport').textContent =
      '==ERROR: AddressSanitizer: heap-use-after-free\n' +
      'READ of size 4 ... thread T0\n' +
      '#0 ... in main ... dangling.cpp:7\n\n' + t().reportNote;
    $('allowed').innerHTML = t().allowed + ' ' +
      '<code>make_unique&lt;Resource&gt;()</code> · ' +
      '<code>auto&amp; r = *owner</code> · ' +
      '<code>use(r)</code> · ' +
      '<code>owner.reset()</code> · ' +
      '<code>return 0;</code>';
    $('footerNote').textContent = t().footer;
    $('articleLink').textContent = t().article;
    $('exampleLink').textContent = t().example;
    updateHp();
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

  function glitch(message) {
    $('arena').classList.remove('glitch');
    void $('arena').offsetWidth;
    $('arena').classList.add('glitch');
    heroHp -= 25;
    log(message, 'miss');
    $('dialogue').textContent = t().miss;
    $('debugger').hidden = false;
    updateHp();
  }

  function resetBattle() {
    heroHp = 100;
    bossHp = 100;
    won = state.cleared.indexOf(QUEST_ID) >= 0;
    $('battleLog').textContent = '';
    $('debugger').hidden = true;
    $('dialogue').textContent = t().intro;
    $('runBtn').textContent = won ? t().cleared : t().run;
    updateHp();
  }

  function runSpell() {
    if (heroHp <= 0 || won) { resetBattle(); return; }

    let alive = false;
    let refBound = false;
    let damaged = 0;
    const lines = $('code').value.split('\n');

    for (const rawLine of lines) {
      const line = rawLine.split('//')[0].trim();
      if (!line) continue;

      if (/make_unique\s*<[^>]*Resource[^>]*>/.test(line) ||
          /make_unique\s*<[^>]*>/.test(line)) {
        alive = true;
        log(state.lang === 'zh' ? '创建 owner：资源在堆上。' : 'owner created: resource on the heap.');
      } else if (/auto\s*&\s*\w+\s*=\s*\*\s*owner/.test(line)) {
        if (!alive) { glitch(t().syntax); return; }
        refBound = true;
        log(state.lang === 'zh' ? '绑定 r：引用指向活对象。' : 'r bound: reference points at a live object.');
      } else if (/owner\s*\.\s*reset\s*\(\s*\)|owner\s*=\s*nullptr/.test(line)) {
        alive = false;
        log(state.lang === 'zh' ? 'owner.reset()：资源已释放。' : 'owner.reset(): resource released.');
      } else if (/use\s*\(\s*\w+\s*\)|cout\s*<<\s*\w+\s*\.\s*value/.test(line)) {
        if (!refBound) { glitch(t().syntax); return; }
        if (!alive) { glitch(t().miss); return; }
        bossHp -= 34;
        damaged += 34;
        log(t().hit, 'hit');
      } else if (/return\s+0\s*;/.test(line)) {
        break;
      } else {
        glitch(t().unknown);
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
    save();
    applyChrome();
  });

  $('langBtn').addEventListener('click', () => {
    state.lang = state.lang === 'en' ? 'zh' : 'en';
    save();
    applyChrome();
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
