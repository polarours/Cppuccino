(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const QUEST_ID = 'challenge-en-boss-data-race';

  const TEXT = {
    en: {
      stage: 'STAGE 04 · BOSS',
      name: 'DATA RACE',
      sub: 'Two threads, one counter, no synchronization. It corrupts your state while you look away.',
      hero: 'YOU',
      intro: 'Write a worker body that two threads can run safely. Guard the shared counter or the race wins.',
      hint: 'Allowed spell: lock(m); ++shared; unlock(m);. Two threads run your lines, so a bare increment races.',
      run: 'RUN ▶',
      retry: 'RETRY ↺',
      cleared: 'CLEARED ✓',
      debug: 'Q · DEBUG',
      debugHead: 'DEBUGGER · THREADSANITIZER REPORT',
      allowed: 'ALLOWED SPELLS:',
      hit: 'HIT! synchronized increment by {thread}. Boss HP -34.',
      miss: 'MISS! {thread} touched shared without the mutex.',
      unknown: 'UNKNOWN SPELL. Check the allowed list below.',
      win: 'STAGE CLEARED. No unsynchronized access remains.',
      lose: 'GAME OVER. The race corrupted everything. Retry.',
      reportNote: 'Captured from a local -fsanitize=thread repro; addresses omitted. Exit code 66.',
      footer: 'The browser runs a small teaching DSL. It cannot compile arbitrary C++; the links above open the real pitfall and example.',
      article: 'READ THE REAL PITFALL ↗',
      example: 'RUNNABLE EXAMPLE ↗',
      locked: '{thread} acquired m.',
      unlocked: '{thread} released m.',
      finished: '{thread} finished its worker body.'
    },
    zh: {
      stage: '第 04 关 · BOSS',
      name: '数据竞争',
      sub: '两个线程、一个计数器、没有同步。它在你眨眼时破坏状态。',
      hero: '你',
      intro: '写一段两个线程都能安全执行的代码。不保护共享计数器，竞争就会赢。',
      hint: '可用咒语：lock(m); ++shared; unlock(m);。两个线程都会执行这几行，裸自增必然竞争。',
      run: '运行 ▶',
      retry: '重试 ↺',
      cleared: '已通关 ✓',
      debug: 'Q · 调试',
      debugHead: '调试器 · ThreadSanitizer 报告',
      allowed: '可用咒语：',
      hit: '命中！{thread} 完成同步自增，Boss 生命 -34。',
      miss: '失手！{thread} 在没有互斥锁的情况下访问了 shared。',
      unknown: '未知咒语。请看下面的可用列表。',
      win: '通关。没有留下未同步访问。',
      lose: '游戏结束。竞争破坏了所有状态，重试。',
      reportNote: '来自本地 -fsanitize=thread 实测；地址已省略，退出码 66。',
      footer: '浏览器里运行的是教学 DSL，不能编译任意 C++；上面链接打开真实陷阱文章和示例。',
      article: '阅读真实陷阱文章 ↗',
      example: '可运行示例 ↗',
      locked: '{thread} 获得 m。',
      unlocked: '{thread} 释放 m。',
      finished: '{thread} 执行完线程体。'
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

  function fmt(template, thread) {
    return template.replace('{thread}', thread);
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
      'WARNING: ThreadSanitizer: data race\n' +
      'Read of size 4 ... by thread T2\n' +
      '#0 operator() ... race.cpp:6\n\n' + t().reportNote;
    $('allowed').innerHTML = t().allowed + ' ' +
      '<code>lock(m)</code> · ' +
      '<code>++shared</code> · ' +
      '<code>unlock(m)</code> · ' +
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
    $('dialogue').textContent = message;
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

    let damaged = 0;
    const lines = $('code').value.split('\n').map((line) => line.split('//')[0].trim()).filter(Boolean);

    if (!lines.some((line) => /\+\+\s*shared|shared\s*\+\+|use\s*\(\s*shared\s*\)/.test(line))) {
      $('dialogue').textContent = t().hint;
      return;
    }

    for (const thread of ['T1', 'T2']) {
      let locked = false;
      for (const line of lines) {
        if (/lock\s*\(\s*m\s*\)|lock_guard/.test(line)) {
          locked = true;
          log(fmt(t().locked, thread));
        } else if (/unlock\s*\(\s*m\s*\)/.test(line)) {
          locked = false;
          log(fmt(t().unlocked, thread));
        } else if (/\+\+\s*shared|shared\s*\+\+|use\s*\(\s*shared\s*\)/.test(line)) {
          if (!locked) {
            glitch(fmt(t().miss, thread));
            return;
          }
          bossHp -= 34;
          damaged += 34;
          log(fmt(t().hit, thread), 'hit');
          if (bossHp <= 0) break;
        } else if (/return\s+0\s*;/.test(line)) {
          break;
        } else {
          glitch(t().unknown);
          return;
        }
      }
      log(fmt(t().finished, thread));
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
