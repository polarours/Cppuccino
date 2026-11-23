(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const QUEST_ID = 'challenge-en-boss-iterator-invalidation';

  const TEXT = {
    en: {
      stage: 'STAGE 04 · BOSS',
      name: 'ITERATOR INVALIDATION',
      sub: 'It turns your iterator into a singular one the moment the vector grows.',
      hero: 'YOU',
      intro: 'Write a spell that never touches a stale iterator. Grow the vector, then rebind or finish before it mutates.',
      hint: 'Allowed spell: bind it, attack with ++it, mutate the vector, rebind with vec.begin(). Repeat ++it to attack.',
      run: 'RUN ▶',
      retry: 'RETRY ↺',
      cleared: 'CLEARED ✓',
      debug: 'Q · DEBUG',
      debugHead: 'DEBUGGER · LIBSTDC++ DEBUG REPORT',
      allowed: 'ALLOWED SPELLS:',
      hit: 'HIT! The iterator is still valid. Boss HP -34.',
      miss: 'MISS! You incremented a singular iterator.',
      unknown: 'UNKNOWN SPELL. Check the allowed list below.',
      unbound: 'NO ITERATOR YET. Bind it with vec.begin() first.',
      win: 'STAGE CLEARED. No iterator was left behind.',
      lose: 'GAME OVER. Every iterator is singular. Retry.',
      reportNote: 'Captured from a local -D_GLIBCXX_DEBUG repro; addresses omitted. Exit code 134.',
      footer: 'The browser runs a small teaching DSL. It cannot compile arbitrary C++; the links above open the real pitfall and example.',
      article: 'READ THE REAL PITFALL ↗',
      example: 'RUNNABLE EXAMPLE ↗',
      bound: 'it bound: iterator points at a live element.',
      mutated: 'vec.push_back() reallocated. Existing iterators are singular now.',
      rebound: 'it rebound with vec.begin() after the mutation.'
    },
    zh: {
      stage: '第 04 关 · BOSS',
      name: '迭代器失效',
      sub: 'vector 一扩容，你的迭代器就变成奇异迭代器。',
      hero: '你',
      intro: '写一段咒语，永远不碰失效的迭代器。要么先攻击再修改容器，要么扩容后重新绑定。',
      hint: '可用咒语：vec.begin() 绑定、++it 攻击、vec.push_back() 修改容器。重复 ++it 可以连续攻击。',
      run: '运行 ▶',
      retry: '重试 ↺',
      cleared: '已通关 ✓',
      debug: 'Q · 调试',
      debugHead: '调试器 · libstdc++ 调试报告',
      allowed: '可用咒语：',
      hit: '命中！迭代器仍然有效，Boss 生命 -34。',
      miss: '失手！你递增了一个奇异迭代器。',
      unknown: '未知咒语。请看下面的可用列表。',
      unbound: '还没有迭代器。先用 vec.begin() 绑定。',
      win: '通关。没有留下任何失效迭代器。',
      lose: '游戏结束。迭代器全失效了，重试。',
      reportNote: '来自本地 -D_GLIBCXX_DEBUG 实测；地址已省略，退出码 134。',
      footer: '浏览器里运行的是教学 DSL，不能编译任意 C++；上面链接打开真实陷阱文章和示例。',
      article: '阅读真实陷阱文章 ↗',
      example: '可运行示例 ↗',
      bound: 'it 已绑定：迭代器指向活元素。',
      mutated: 'vec.push_back() 触发扩容，旧迭代器变成奇异迭代器。',
      rebound: '扩容后用 vec.begin() 重新绑定 it。'
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
      '/usr/include/c++/16/debug/safe_iterator.h:392\n' +
      'Error: attempt to increment a singular iterator.\n\n' + t().reportNote;
    $('allowed').innerHTML = t().allowed + ' ' +
      '<code>auto it = vec.begin()</code> · ' +
      '<code>++it</code> · ' +
      '<code>vec.push_back(4)</code> · ' +
      '<code>it = vec.begin()</code> · ' +
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

    let bound = false;
    let valid = false;
    let damaged = 0;
    const lines = $('code').value.split('\n');

    for (const rawLine of lines) {
      const line = rawLine.split('//')[0].trim();
      if (!line) continue;

      if (/vec\s*\.\s*begin\s*\(\s*\)/.test(line)) {
        bound = true;
        valid = true;
        log(t().bound);
      } else if (/vec\s*\.\s*push_back\s*\(/.test(line)) {
        if (bound) valid = false;
        log(t().mutated);
      } else if (/\+\s*\+?\s*it|it\s*\+\+/.test(line)) {
        if (!bound) { glitch(t().unbound); return; }
        if (!valid) { glitch(t().miss); return; }
        bossHp -= 34;
        damaged += 34;
        log(t().hit, 'hit');
      } else if (/use\s*\(\s*it\s*\)|cout\s*<<\s*\*\s*it/.test(line)) {
        if (!bound) { glitch(t().unbound); return; }
        if (!valid) { glitch(t().miss); return; }
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
