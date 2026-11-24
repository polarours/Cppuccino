(function () {
  'use strict';

  const STORE = 'cppuccino-arcade-v1';
  const STRINGS = {
    en: {
      heroEyebrow: 'INSERT CURIOSITY · PRESS START',
      heroTitle: 'SELECT YOUR STAGE',
      heroSub: 'One C++ idea, in two languages, with runnable proof. Clear a stage by reading, fighting a bug, and shipping the code.',
      campLabel: 'UB TRAINING CAMP',
      campTitle: 'Four bosses. One learning loop.',
      campCta: 'ENTER CAMP ↗',
      level: 'LEVEL', xp: 'XP', cleared: 'CLEARED', stages: 'STAGES',
      worldsTitle: 'WORLD MAP',
      worldsHint: 'Every stage points at real articles, pitfalls and runnable examples.',
      back: '← WORLD MAP',
      footer: 'Progress stays in this browser. No account, no leaderboard.',
      enter: 'ENTER STAGE →',
      clear: 'CLEAR', undo: 'UNDO',
      group: { intel: 'INTEL', boss: 'BOSS', loot: 'LOOT', challenge: 'CLEAR' },
      stage: 'STAGE',
      error: 'Could not load quest data. Reload the page.'
    },
    zh: {
      heroEyebrow: '投入好奇心 · 按 START',
      heroTitle: '选择你的关卡',
      heroSub: '一个 C++ 想法，两种语言，配一份能跑起来的证明。读文章、打 Bug、跑通代码，就算过关。',
      campLabel: 'UB 训练营',
      campTitle: '四个 Boss，一个学习循环。',
      campCta: '进入训练营 ↗',
      level: '等级', xp: '经验', cleared: '已通关', stages: '关卡',
      worldsTitle: '世界地图',
      worldsHint: '每个关卡都指向真实的文章、陷阱和可运行示例。',
      back: '← 返回地图',
      footer: '进度只保存在本浏览器，没有账号，也没有排行榜。',
      enter: '进入关卡 →',
      clear: '通关', undo: '撤销',
      group: { intel: '情报', boss: 'BOSS', loot: '掉落', challenge: '挑战' },
      stage: '第',
      error: '关卡数据加载失败，请刷新页面。'
    }
  };

  const defaults = { theme: 'arcade', lang: 'en', cleared: [], stage: null };
  let state = Object.assign({}, defaults);
  let data = null;

  try {
    const saved = JSON.parse(localStorage.getItem(STORE) || '{}');
    state = Object.assign({}, defaults, saved);
    if (!Array.isArray(state.cleared)) state.cleared = [];
  } catch (_) {
    state = Object.assign({}, defaults);
  }

  const $ = (id) => document.getElementById(id);
  const pad = (n) => String(n).padStart(2, '0');
  const s = () => STRINGS[state.lang];

  function save() {
    try { localStorage.setItem(STORE, JSON.stringify(state)); } catch (_) {}
  }

  function applyChrome() {
    const root = document.documentElement;
    root.dataset.theme = state.theme;
    root.dataset.lang = state.lang;
    $('themeBtn').textContent = state.theme === 'arcade' ? 'RETRO' : 'ARCADE';
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('heroEyebrow').textContent = s().heroEyebrow;
    $('heroTitle').textContent = s().heroTitle;
    $('heroSub').textContent = s().heroSub;
    $('campLabel').textContent = s().campLabel;
    $('campTitle').textContent = s().campTitle;
    $('campCta').textContent = s().campCta;
    $('labelLevel').textContent = s().level;
    $('labelXp').textContent = s().xp;
    $('labelCleared').textContent = s().cleared;
    $('labelStages').textContent = s().stages;
    $('worldsTitle').textContent = s().worldsTitle;
    $('worldsHint').textContent = s().worldsHint;
    $('backBtn').textContent = s().back;
    $('footerNote').textContent = s().footer;
    document.title = 'Cppuccino Arcade — ' + s().heroTitle;
  }

  function renderHud() {
    const totalXp = data.quests.reduce((sum, q) =>
      sum + (state.cleared.indexOf(q.id) >= 0 ? q.xp : 0), 0);
    const level = Math.floor(totalXp / 100) + 1;
    $('hudLevel').textContent = pad(level);
    $('hudXp').textContent = totalXp;
    $('hudCleared').textContent = state.cleared.length + ' / ' + data.quests.length;
    $('hudStages').textContent = pad(data.worlds.length);
  }

  function worldQuests(key) {
    return data.quests.filter((q) => q.world === key);
  }

  function renderWorlds() {
    const host = $('worlds');
    host.innerHTML = '';
    data.worlds.forEach((world, index) => {
      const quests = worldQuests(world.key);
      const done = quests.filter((q) => state.cleared.indexOf(q.id) >= 0).length;
      const pct = quests.length ? Math.round((done / quests.length) * 100) : 0;
      const button = document.createElement('button');
      button.type = 'button';
      button.className = 'world';
      button.setAttribute('aria-label',
        (state.lang === 'zh' ? world.zh : world.en) + ' — ' + done + '/' + quests.length);
      button.innerHTML =
        '<span class="world-code">STG-' + pad(index + 1) + '</span>' +
        '<span class="world-title">' + (state.lang === 'zh' ? world.zh : world.en) + '</span>' +
        '<span class="world-tagline">' + (state.lang === 'zh' ? world.tagline_zh : world.tagline_en) + '</span>' +
        '<span class="meter"><span style="width:' + pct + '%"></span></span>' +
        '<span class="world-meta"><span>' + done + '/' + quests.length + '</span><span>' + pct + '%</span></span>' +
        '<span class="world-enter">' + s().enter + '</span>';
      button.addEventListener('click', () => openStage(world.key));
      host.appendChild(button);
    });
  }

  function renderQuest(quest) {
    const row = document.createElement('div');
    row.className = 'quest';
    const cleared = state.cleared.indexOf(quest.id) >= 0;
    const link = document.createElement('a');
    link.href = quest.url;
    link.target = '_blank';
    link.rel = 'noopener';
    link.textContent = quest.title || quest.id;
    const xp = document.createElement('span');
    xp.className = 'xp';
    xp.textContent = '+' + quest.xp + ' XP · ' + (quest.lang === 'zh' ? '中文' : 'EN');
    link.appendChild(xp);
    const button = document.createElement('button');
    button.type = 'button';
    button.textContent = cleared ? s().undo : s().clear;
    button.setAttribute('aria-pressed', cleared ? 'true' : 'false');
    button.addEventListener('click', () => {
      const i = state.cleared.indexOf(quest.id);
      if (i >= 0) state.cleared.splice(i, 1);
      else state.cleared.push(quest.id);
      save();
      renderHud();
      renderWorlds();
      renderStage(state.stage);
    });
    row.appendChild(link);
    row.appendChild(button);
    return row;
  }

  function renderStage(key) {
    if (!key) return;
    const world = data.worlds.find((w) => w.key === key);
    if (!world) return;
    state.stage = key;
    save();

    $('worlds').hidden = true;
    $('stage').hidden = false;
    const index = data.worlds.indexOf(world) + 1;
    $('stageCode').textContent = s().stage + ' ' + pad(index);
    $('stageTitle').textContent = state.lang === 'zh' ? world.zh : world.en;
    $('stageTagline').textContent = state.lang === 'zh' ? world.tagline_zh : world.tagline_en;

    const host = $('stageGroups');
    host.innerHTML = '';
    ['intel', 'boss', 'loot', 'challenge'].forEach((kind) => {
      const quests = worldQuests(key).filter((q) => q.kind === kind);
      if (!quests.length) return;
      const group = document.createElement('div');
      const head = document.createElement('div');
      head.className = 'group-head';
      head.innerHTML = '<span>' + s().group[kind] + '</span><small>' + quests.length + '</small>';
      group.appendChild(head);
      quests.forEach((q) => group.appendChild(renderQuest(q)));
      host.appendChild(group);
    });
  }

  function openStage(key) {
    renderStage(key);
    window.scrollTo({ top: 0, behavior: 'smooth' });
  }

  function showMap() {
    state.stage = null;
    save();
    $('stage').hidden = true;
    $('worlds').hidden = false;
    renderWorlds();
  }

  $('themeBtn').addEventListener('click', () => {
    state.theme = state.theme === 'arcade' ? 'retro' : 'arcade';
    save();
    applyChrome();
  });

  $('langBtn').addEventListener('click', () => {
    state.lang = state.lang === 'en' ? 'zh' : 'en';
    save();
    applyChrome();
    renderWorlds();
    if (state.stage) renderStage(state.stage);
  });

  $('backBtn').addEventListener('click', showMap);

  fetch('../assets/quests.json')
    .then((r) => {
      if (!r.ok) throw new Error(r.status);
      return r.json();
    })
    .then((json) => {
      data = json;
      applyChrome();
      renderHud();
      if (state.stage) renderStage(state.stage);
      else renderWorlds();
    })
    .catch(() => {
      $('worlds').textContent = s().error;
    });
})();
