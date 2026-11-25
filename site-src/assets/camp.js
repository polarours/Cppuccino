(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const CAMP_KEY = 'cppuccino-camp-v1';
  const PROOF_KEY = 'cppuccino-proof-v1';
  const BOSSES = [
    'challenge-en-boss-dangling-pointer',
    'challenge-en-boss-iterator-invalidation',
    'challenge-en-boss-data-race',
    'challenge-en-boss-double-free'
  ];
  const STEPS = ['read', 'lab', 'fight', 'prove'];

  const TEXT = {
    en: {
      progressLabel: 'BOSSES CLEARED',
      progressHint: 'Each card tracks READ → LAB → FIGHT → PROVE. Progress stays in this browser.',
      progressDone: 'Camp cleared. Four failure classes, four real sanitizer reports.',
      open: 'OPEN',
      inProgress: 'IN PROGRESS',
      cleared: 'CLEARED',
      nextStep: 'NEXT STEP',
      next: { read: 'READ', lab: 'LAB', fight: 'FIGHT', prove: 'PROVE' },
      continue: 'CONTINUE ↗',
      campCompleteTitle: 'FOUR PROOFS STAMPED',
      reviewProofs: 'REVIEW PROOFS ↗',
      noteTitle: 'WHY THIS CAMP EXISTS',
      noteBody: 'Every boss prints a sanitizer report captured from a local reproduction in this repository. The browser runs a small teaching DSL; the article and example are the real C++. Clear all four and you have a working mental model of lifetime, container, concurrency, and ownership bugs.',
      footer: 'No account, no leaderboard. Progress is local to this browser.',
      ubLink: 'READ THE UB OVERVIEW ↗'
    },
    zh: {
      progressLabel: '已击败 BOSS',
      progressHint: '每张卡跟踪 阅读 → 实验 → 战斗 → 证明。进度只保存在当前浏览器。',
      progressDone: '训练营通关。四类失败模式，四份真实 sanitizer 报告。',
      open: '待挑战',
      inProgress: '进行中',
      cleared: '已通关',
      nextStep: '下一步',
      next: { read: '阅读', lab: '实验', fight: '战斗', prove: '证明' },
      continue: '继续 ↗',
      campCompleteTitle: '四份证明已盖章',
      reviewProofs: '查看证明 ↗',
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

  let camp = { read: {}, lab: {} };
  let proofs = {};
  function loadCamp() {
    try {
      const saved = JSON.parse(localStorage.getItem(CAMP_KEY) || '{}');
      camp = {
        read: saved.read || {},
        lab: saved.lab || {}
      };
    } catch (_) {
      camp = { read: {}, lab: {} };
    }
    try {
      proofs = JSON.parse(localStorage.getItem(PROOF_KEY) || '{}');
    } catch (_) {
      proofs = {};
    }
  }

  const $ = (id) => document.getElementById(id);
  const t = () => TEXT[state.lang];

  function stepStates(id) {
    const read = !!camp.read[id];
    const lab = !!camp.lab[id];
    const fight = state.cleared.indexOf(id) >= 0;
    const proof = !!(proofs[id] && proofs[id].passed);
    return { read: read, lab: lab, fight: fight, prove: read && lab && fight && proof };
  }

  function save() {
    try { localStorage.setItem(STORE, JSON.stringify(state)); } catch (_) {}
  }

  function clearedCount() {
    return BOSSES.filter((id) => state.cleared.indexOf(id) >= 0).length;
  }

  function applyLanguage() {
    document.documentElement.dataset.lang = state.lang;
    document.documentElement.setAttribute(
      'lang', state.lang === 'zh' ? 'zh-CN' : 'en');
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
    loadCamp();
    const count = clearedCount();
    $('progressText').textContent = count + ' / ' + BOSSES.length;
    $('progressBar').style.width = (count / BOSSES.length * 100) + '%';
    $('progressHint').textContent = count === BOSSES.length
      ? t().progressDone : t().progressHint;

    document.querySelectorAll('.camp-card').forEach((card) => {
      const id = card.dataset.quest;
      const states = stepStates(id);
      const completed = STEPS.filter((step) => states[step]).length;
      const next = STEPS.find((step) => !states[step]);

      card.classList.toggle('cleared', states.prove);
      card.querySelectorAll('.camp-card-steps span').forEach((span) => {
        span.classList.toggle('done', !!states[span.dataset.step]);
        span.classList.toggle('current', span.dataset.step === next);
      });

      const status = card.querySelector('.camp-card-status');
      status.textContent = states.prove
        ? t().cleared
        : (completed ? t().inProgress : t().open);
    });

    const allDone = BOSSES.every((id) => stepStates(id).prove);
    const nextBox = $('campNext');
    nextBox.classList.toggle('complete', allDone);

    if (allDone) {
      $('campNextKicker').textContent = t().cleared;
      $('campNextTitle').textContent = t().campCompleteTitle;
      $('campNextLink').href = 'prove/';
      $('campNextLink').textContent = t().reviewProofs;
    } else {
      const nextBoss = BOSSES.find((id) => !stepStates(id).prove);
      const states = stepStates(nextBoss);
      const step = STEPS.find((name) => !states[name]);
      const card = document.querySelector('.camp-card[data-quest="' + nextBoss + '"]');
      const selector = (step === 'read' || step === 'lab')
        ? 'a[data-article-en]'
        : (step === 'fight' ? 'a[href*="boss"]' : 'a[href*="prove"]');
      const target = card && card.querySelector(selector);
      const name = card && card.querySelector('h2') ? card.querySelector('h2').textContent : nextBoss;

      $('campNextKicker').textContent = t().nextStep;
      $('campNextTitle').textContent = t().next[step] + ' · ' + name;
      if (target) {
        $('campNextLink').href = target.href;
        $('campNextLink').textContent = t().continue;
      }
    }

    $('campComplete').hidden = !allDone;
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
  window.addEventListener('pageshow', applyProgress);
})();
