(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const BOSSES = [
    'challenge-en-boss-dangling-pointer',
    'challenge-en-boss-iterator-invalidation',
    'challenge-en-boss-data-race',
    'challenge-en-boss-double-free'
  ];

  const TEXT = {
    en: {
      progressLabel: 'BOSSES CLEARED',
      progressHint: 'Progress is stored in this browser by the arcade.',
      progressDone: 'Camp cleared. Four failure classes, four real sanitizer reports.',
      open: 'OPEN',
      cleared: 'CLEARED',
      noteTitle: 'WHY THIS CAMP EXISTS',
      noteBody: 'Every boss prints a sanitizer report captured from a local reproduction in this repository. The browser runs a small teaching DSL; the article and example are the real C++. Clear all four and you have a working mental model of lifetime, container, concurrency, and ownership bugs.',
      footer: 'No account, no leaderboard. Progress is local to this browser.',
      ubLink: 'READ THE UB OVERVIEW ↗'
    },
    zh: {
      progressLabel: '已击败 BOSS',
      progressHint: '进度由街机模式保存在当前浏览器。',
      progressDone: '训练营通关。四类失败模式，四份真实 sanitizer 报告。',
      open: '待挑战',
      cleared: '已通关',
      noteTitle: '为什么有这个训练营',
      noteBody: '每个 Boss 打印的 sanitizer 报告，都来自本仓库中的一次本地真实复现。浏览器里运行的是教学 DSL；文章和示例才是真正的 C++。四个都通关，你就有了生命周期、容器、并发、所有权四类 bug 的工作心智模型。',
      footer: '没有账号，没有排行榜。进度只存在当前浏览器。',
      ubLink: '阅读未定义行为总览 ↗'
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

  function save() {
    try { localStorage.setItem(STORE, JSON.stringify(state)); } catch (_) {}
  }

  function clearedCount() {
    return BOSSES.filter((id) => state.cleared.indexOf(id) >= 0).length;
  }

  function applyLanguage() {
    document.documentElement.dataset.lang = state.lang;
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';

    document.querySelectorAll('[data-en]').forEach((element) => {
      const value = element.dataset[state.lang];
      if (typeof value === 'string') element.textContent = value;
    });

    document.querySelectorAll('[data-article-en]').forEach((link) => {
      const value = link.dataset[state.lang === 'zh' ? 'articleZh' : 'articleEn'];
      if (value) link.href = value;
    });

    $('progressLabel').textContent = t().progressLabel;
    $('progressHint').textContent = clearedCount() === BOSSES.length
      ? t().progressDone : t().progressHint;
    $('noteTitle').textContent = t().noteTitle;
    $('noteBody').textContent = t().noteBody;
    $('noteLink').textContent = t().ubLink;
    $('noteLink').href = state.lang === 'zh'
      ? '../../docs/中文版/未定义行为/'
      : '../../docs/en/undefined-behavior/';
    $('footerNote').textContent = t().footer;
  }

  function applyProgress() {
    const count = clearedCount();
    $('progressText').textContent = count + ' / ' + BOSSES.length;
    $('progressBar').style.width = (count / BOSSES.length * 100) + '%';
    $('progressHint').textContent = count === BOSSES.length
      ? t().progressDone : t().progressHint;

    document.querySelectorAll('.camp-card').forEach((card) => {
      const done = state.cleared.indexOf(card.dataset.quest) >= 0;
      card.classList.toggle('cleared', done);
      const status = card.querySelector('.camp-card-status');
      status.textContent = done ? t().cleared : t().open;
    });
  }

  function applyTheme() {
    document.documentElement.dataset.theme = state.theme;
    $('themeBtn').textContent = state.theme === 'arcade' ? 'RETRO' : 'ARCADE';
  }

  $('themeBtn').addEventListener('click', () => {
    state.theme = state.theme === 'arcade' ? 'retro' : 'arcade';
    save(); applyTheme();
  });

  $('langBtn').addEventListener('click', () => {
    state.lang = state.lang === 'en' ? 'zh' : 'en';
    save(); applyLanguage(); applyProgress();
  });

  applyTheme();
  applyLanguage();
  applyProgress();
})();
