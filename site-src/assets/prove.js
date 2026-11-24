(function () {
  'use strict';

  const PROOF_KEY = 'cppuccino-proof-v1';
  const ARCADE_KEY = 'cppuccino-arcade-v1';
  const THEME_KEY = 'cppuccino-read-theme';
  const LANG_KEY = 'cppuccino-lab-lang';

  const TEXT = {
    en: {
      eyebrow: 'CAMP FINAL · READ → LAB → FIGHT → PROVE',
      title: 'PROOF GATE',
      sub: 'Three questions per boss. Prove you can name the failure, explain the repair, and choose the safe code.',
      pickerTitle: 'CHOOSE A PROOF',
      pickerSub: 'Each gate unlocks after its boss is cleared. Three correct answers stamp the proof.',
      lockedTitle: 'BOSS STILL STANDING',
      lockedBody: 'Clear this boss before you try to prove the repair. The camp card will move from FIGHT to PROVE once the gate is passed.',
      goBoss: 'GO TO BOSS ↗',
      backCamp: '← BACK TO CAMP',
      question: 'QUESTION',
      of: '/',
      score: 'SCORE',
      correct: 'CORRECT',
      wrong: 'NOT QUITE',
      next: 'NEXT →',
      finish: 'FINISH →',
      passedTitle: 'PROOF STAMPED',
      passedBody: 'Three correct answers. The camp card can now reach PROVE and CLEARED.',
      failedTitle: 'PROOF NOT YET',
      failedBody: 'You need all three answers correct. Read the explanation, then retry the gate.',
      retry: 'RETRY ↺',
      open: 'OPEN',
      proofed: 'PROOFED',
      locked: 'LOCKED',
      footer: 'Proof results stay in this browser. No account, no leaderboard.'
    },
    zh: {
      eyebrow: '训练营终局 · 阅读 → 实验 → 战斗 → 证明',
      title: '证明关卡',
      sub: '每个 Boss 三道题。证明你能说出失败模式、解释修复，并选出安全代码。',
      pickerTitle: '选择要证明的 Boss',
      pickerSub: '击败对应 Boss 后解锁；三道全对才算盖章。',
      lockedTitle: 'Boss 还没倒下',
      lockedBody: '先击败这个 Boss，再来证明修复。通过后，训练营卡片才会从战斗推进到证明。',
      goBoss: '去打 Boss ↗',
      backCamp: '← 返回训练营',
      question: '第',
      of: '/',
      score: '得分',
      correct: '正确',
      wrong: '还差一点',
      next: '下一题 →',
      finish: '完成 →',
      passedTitle: '证明已盖章',
      passedBody: '三道全对。训练营卡片现在可以推进到“证明”和“已通关”。',
      failedTitle: '还没通过',
      failedBody: '需要三道全对。看完解释后重试这一关。',
      retry: '重试 ↺',
      open: '待证明',
      proofed: '已盖章',
      locked: '未解锁',
      footer: '证明结果只保存在当前浏览器。没有账号，没有排行榜。'
    }
  };

  const BOSSES = [
    {
      slug: 'dangling-pointer',
      quest: 'challenge-en-boss-dangling-pointer',
      name: 'DANGLING POINTER',
      tag: { en: 'LIFETIME', zh: '生命周期' },
      article: '../../../pitfalls/en/dangling-pointer/',
      bossUrl: '../../boss/',
      questions: [
        {
          q: {
            en: 'Which line leaves the reference r dangling?',
            zh: '哪一行会让引用 r 悬垂？'
          },
          choices: [
            { en: 'auto p = std::make_unique<Resource>(); auto& r = *p;', zh: 'auto p = std::make_unique<Resource>(); auto& r = *p;' },
            { en: 'auto& r = *std::make_unique<Resource>();', zh: 'auto& r = *std::make_unique<Resource>();' },
            { en: 'Resource x; auto& r = x;', zh: 'Resource x; auto& r = x;' }
          ],
          answer: 1,
          why: {
            en: 'The temporary unique_ptr dies at the end of the full expression, so the Resource is freed while r still refers to it.',
            zh: '临时 unique_ptr 在完整表达式结束时销毁，Resource 被释放，而 r 仍然引用它。'
          }
        },
        {
          q: {
            en: 'After owner.reset(), what does r refer to?',
            zh: 'owner.reset() 之后，r 引用的是什么？'
          },
          choices: [
            { en: 'A live Resource', zh: '一个仍然存活的 Resource' },
            { en: 'The same Resource, now owned by r', zh: '同一个 Resource，现在归 r 所有' },
            { en: 'A destroyed object; any use is undefined behavior', zh: '一个已销毁的对象；任何使用都是未定义行为' }
          ],
          answer: 2,
          why: {
            en: 'A reference does not own or extend lifetime. Once the owner resets, the referred object is gone and r dangles.',
            zh: '引用不拥有对象，也不延长生命周期。所有者 reset 后对象已经没了，r 就是悬垂引用。'
          }
        },
        {
          q: {
            en: 'Which repair keeps later reads safe when ownership may change?',
            zh: '所有权可能变化时，哪种修复能让后续读取保持安全？'
          },
          choices: [
            { en: 'Copy the reference into a raw pointer', zh: '把引用复制成裸指针' },
            { en: 'Reorder reset() after the last use, or hold a shared_ptr/weak_ptr and lock it', zh: '把 reset() 移到最后一次使用之后，或持有 shared_ptr/weak_ptr 并 lock' },
            { en: 'Call delete on the reference', zh: '对引用调用 delete' }
          ],
          answer: 1,
          why: {
            en: 'Shorten the reference lifetime or make ownership explicit. A raw pointer copy does not keep the object alive.',
            zh: '缩短引用生命周期，或把所有权显式化。复制裸指针并不会让对象继续存活。'
          }
        }
      ]
    },
    {
      slug: 'iterator-invalidation',
      quest: 'challenge-en-boss-iterator-invalidation',
      name: 'ITERATOR INVALIDATION',
      tag: { en: 'CONTAINERS', zh: '容器' },
      article: '../../../pitfalls/en/iterator-invalidation/',
      bossUrl: '../../boss/iterator-invalidation/',
      questions: [
        {
          q: {
            en: 'auto it = v.begin(); v.push_back(4); — what can happen to it?',
            zh: 'auto it = v.begin(); v.push_back(4); —— it 可能发生什么？'
          },
          choices: [
            { en: 'It stays valid and points into the new buffer', zh: '它保持有效，并指向新缓冲区' },
            { en: 'It may be invalidated; dereferencing it is undefined behavior', zh: '它可能失效；解引用它是未定义行为' },
            { en: 'The vector automatically rebinds it', zh: 'vector 会自动重新绑定它' }
          ],
          answer: 1,
          why: {
            en: 'If push_back reallocates, the old buffer is freed and every iterator into it becomes invalid.',
            zh: '如果 push_back 触发扩容，旧缓冲区被释放，指向它的迭代器全部失效。'
          }
        },
        {
          q: {
            en: 'Which operation can invalidate all iterators by moving the buffer?',
            zh: '哪种操作可能搬走缓冲区并使所有迭代器失效？'
          },
          choices: [
            { en: 'v.size()', zh: 'v.size()' },
            { en: 'v.push_back(x) when size == capacity', zh: 'size == capacity 时的 v.push_back(x)' },
            { en: 'v.front()', zh: 'v.front()' }
          ],
          answer: 1,
          why: {
            en: 'Growing past capacity forces reallocation. size(), front() and other observers do not move the buffer.',
            zh: '超过容量继续增长会强制扩容。size()、front() 等观察操作不会搬走缓冲区。'
          }
        },
        {
          q: {
            en: 'What is the safe recovery after a mutation that may reallocate?',
            zh: '可能扩容的修改之后，安全的恢复方式是什么？'
          },
          choices: [
            { en: 'it = v.begin(); reacquire the iterator after the mutation', zh: 'it = v.begin(); 在修改后重新取得迭代器' },
            { en: '++it;', zh: '++it;' },
            { en: 'delete &*it;', zh: 'delete &*it;' }
          ],
          answer: 0,
          why: {
            en: 'Reacquire handles from the container after a structural change. Advancing or deleting through a stale iterator is still UB.',
            zh: '结构变化后要从容器重新取得句柄。通过失效迭代器前进或删除仍然是 UB。'
          }
        }
      ]
    },
    {
      slug: 'data-race',
      quest: 'challenge-en-boss-data-race',
      name: 'DATA RACE',
      tag: { en: 'CONCURRENCY', zh: '并发' },
      article: '../../../pitfalls/en/data-race/',
      bossUrl: '../../boss/data-race/',
      questions: [
        {
          q: {
            en: 'Which situation is a data race?',
            zh: '哪种情况属于数据竞争？'
          },
          choices: [
            { en: 'Two threads read the same const value', zh: '两个线程读取同一个 const 值' },
            { en: 'Two threads access the same int, at least one writes, with no synchronization', zh: '两个线程访问同一个 int，至少一个写，且没有同步' },
            { en: 'Two threads use two different mutexes', zh: '两个线程使用两把不同的互斥锁' }
          ],
          answer: 1,
          why: {
            en: 'A data race needs concurrent access to the same location, at least one write, and no synchronization ordering the accesses.',
            zh: '数据竞争需要并发访问同一位置、至少一个写操作，并且没有同步来排序这些访问。'
          }
        },
        {
          q: {
            en: 'Two threads each run ++shared from 0. Why can the final value be 1?',
            zh: '两个线程都从 0 开始执行 ++shared。为什么最终可能是 1？'
          },
          choices: [
            { en: 'The compiler always removes one increment', zh: '编译器总会删掉一次自增' },
            { en: 'Both read 0 before either writes, so one update overwrites the other', zh: '两边都在对方写入前读到 0，一次更新覆盖了另一次' },
            { en: 'int cannot reach 2', zh: 'int 无法到达 2' }
          ],
          answer: 1,
          why: {
            en: 'This is a lost update: read-modify-write is not atomic, so both threads can compute 0 + 1.',
            zh: '这是丢失更新：读-改-写不是原子操作，两个线程都可能算出 0 + 1。'
          }
        },
        {
          q: {
            en: 'Which repair makes ++shared safe for two threads?',
            zh: '哪种修复能让 ++shared 在两个线程下安全？'
          },
          choices: [
            { en: 'volatile int shared', zh: 'volatile int shared' },
            { en: 'std::atomic<int> shared, or a mutex around the increment', zh: 'std::atomic<int> shared，或用互斥锁包住自增' },
            { en: 'const int shared', zh: 'const int shared' }
          ],
          answer: 1,
          why: {
            en: 'volatile is not synchronization. Use an atomic type or a mutex to make the read-modify-write safe.',
            zh: 'volatile 不是同步。用原子类型或互斥锁让读-改-写安全。'
          }
        }
      ]
    },
    {
      slug: 'double-free',
      quest: 'challenge-en-boss-double-free',
      name: 'DOUBLE FREE',
      tag: { en: 'OWNERSHIP', zh: '所有权' },
      article: '../../../pitfalls/en/double-free/',
      bossUrl: '../../boss/double-free/',
      questions: [
        {
          q: {
            en: 'What is wrong with delete p; delete p;?',
            zh: 'delete p; delete p; 的问题是什么？'
          },
          choices: [
            { en: 'Nothing: delete sets p to null automatically', zh: '没问题：delete 会自动把 p 置空' },
            { en: 'Undefined behavior: the same allocation is released twice', zh: '未定义行为：同一个分配被释放两次' },
            { en: 'It is a compile error', zh: '这是编译错误' }
          ],
          answer: 1,
          why: {
            en: 'delete does not null the pointer. The second delete hands an already-freed block back to the allocator.',
            zh: 'delete 不会把指针置空。第二次 delete 把已释放的块再次交还给分配器。'
          }
        },
        {
          q: {
            en: 'After delete p, what makes a later delete p safe?',
            zh: 'delete p 之后，怎样让后续的 delete p 安全？'
          },
          choices: [
            { en: 'p = nullptr;', zh: 'p = nullptr;' },
            { en: 'p = new Resource();', zh: 'p = new Resource();' },
            { en: 'Nothing: delete is always safe', zh: '什么都不用：delete 总是安全' }
          ],
          answer: 0,
          why: {
            en: 'Deleting nullptr is a no-op. Assigning a new allocation would make the pointer own a different object, not repair the old one.',
            zh: 'delete nullptr 是空操作。重新 new 只是让指针拥有另一个对象，并不能修复旧对象。'
          }
        },
        {
          q: {
            en: 'Which ownership type best prevents accidental double delete?',
            zh: '哪种所有权类型最能防止意外重复释放？'
          },
          choices: [
            { en: 'A raw pointer shared by two owners', zh: '两个所有者共享的裸指针' },
            { en: 'std::unique_ptr', zh: 'std::unique_ptr' },
            { en: 'int*', zh: 'int*' }
          ],
          answer: 1,
          why: {
            en: 'unique_ptr has a single owner and deletes once. Raw pointers do not encode ownership or release responsibility.',
            zh: 'unique_ptr 只有一个所有者，并且只释放一次。裸指针不表达所有权和释放责任。'
          }
        }
      ]
    }
  ];

  let state = {
    lang: 'en',
    theme: 'arcade',
    boss: null,
    qIndex: 0,
    selected: null,
    answers: [],
    score: 0,
    finished: false,
    passed: false
  };

  try {
    state.lang = localStorage.getItem(LANG_KEY) === 'zh' ? 'zh' : 'en';
    const savedTheme = localStorage.getItem(THEME_KEY);
    if (['cappuccino', 'arcade', 'retro'].indexOf(savedTheme) >= 0) {
      state.theme = savedTheme;
    }
  } catch (_) {}

  const $ = (id) => document.getElementById(id);
  const t = () => TEXT[state.lang];
  const textOf = (value) => value && typeof value === 'object' ? value[state.lang] : value;
  const esc = (value) => String(value)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');

  function save() {
    try {
      localStorage.setItem(LANG_KEY, state.lang);
      localStorage.setItem(THEME_KEY, state.theme);
    } catch (_) {}
  }

  function readJson(key) {
    try { return JSON.parse(localStorage.getItem(key) || '{}'); } catch (_) { return {}; }
  }

  function clearState() {
    const saved = readJson(ARCADE_KEY);
    return Array.isArray(saved.cleared) ? saved.cleared : [];
  }

  function proofState() {
    return readJson(PROOF_KEY);
  }

  function isCleared(boss) {
    return clearState().indexOf(boss.quest) >= 0;
  }

  function isProofed(boss) {
    const record = proofState()[boss.quest];
    return !!(record && record.passed);
  }

  function bossBySlug(slug) {
    return BOSSES.find((boss) => boss.slug === slug) || null;
  }

  function applyChrome() {
    document.documentElement.dataset.theme = state.theme;
    document.documentElement.dataset.lang = state.lang;
    $('themeBtn').textContent = state.theme === 'arcade' ? 'RETRO' : 'ARCADE';
    $('langBtn').textContent = state.lang === 'en' ? '中文' : 'EN';
    $('proofEyebrow').textContent = t().eyebrow;
    $('proofTitle').textContent = t().title;
    $('proofSub').textContent = t().sub;
    $('footerNote').textContent = t().footer;
  }

  function renderPicker() {
    const cards = BOSSES.map((boss) => {
      const cleared = isCleared(boss);
      const proofed = isProofed(boss);
      const status = proofed ? t().proofed : (cleared ? t().open : t().locked);
      const stateClass = proofed ? 'proofed' : (cleared ? 'open' : 'locked');
      return '<a class="proof-boss ' + stateClass + '" href="?boss=' + boss.slug + '">' +
        '<span class="proof-boss-tag">' + esc(textOf(boss.tag)) + '</span>' +
        '<strong>' + esc(boss.name) + '</strong>' +
        '<span class="proof-boss-status">' + esc(status) + '</span>' +
        '</a>';
    }).join('');
    $('proofApp').innerHTML =
      '<section class="proof-picker">' +
      '<div class="proof-section-head"><h2>' + t().pickerTitle + '</h2><p>' + t().pickerSub + '</p></div>' +
      '<div class="proof-grid">' + cards + '</div>' +
      '<a class="proof-back" href="../">' + t().backCamp + '</a>' +
      '</section>';
  }

  function renderLocked(boss) {
    $('proofApp').innerHTML =
      '<section class="proof-panel proof-locked">' +
      '<p class="proof-kicker">' + esc(textOf(boss.tag)) + '</p>' +
      '<h2>' + esc(boss.name) + '</h2>' +
      '<h3>' + t().lockedTitle + '</h3>' +
      '<p>' + t().lockedBody + '</p>' +
      '<div class="proof-actions">' +
      '<a class="proof-primary" href="' + boss.bossUrl + '">' + t().goBoss + '</a>' +
      '<a href="../">' + t().backCamp + '</a>' +
      '</div>' +
      '</section>';
  }

  function renderDone(boss, score) {
    $('proofApp').innerHTML =
      '<section class="proof-panel proof-passed">' +
      '<p class="proof-kicker">' + esc(textOf(boss.tag)) + ' · ' + esc(boss.name) + '</p>' +
      '<div class="proof-stamp">' + t().proofed + '</div>' +
      '<h2>' + t().passedTitle + '</h2>' +
      '<p>' + t().passedBody + '</p>' +
      '<p class="proof-score">' + t().score + ' ' + score + ' / 3</p>' +
      '<div class="proof-actions">' +
      '<a class="proof-primary" href="../">' + t().backCamp + '</a>' +
      '</div>' +
      '</section>';
  }

  function renderFailed(boss, score) {
    $('proofApp').innerHTML =
      '<section class="proof-panel proof-failed">' +
      '<p class="proof-kicker">' + esc(textOf(boss.tag)) + ' · ' + esc(boss.name) + '</p>' +
      '<h2>' + t().failedTitle + '</h2>' +
      '<p>' + t().failedBody + '</p>' +
      '<p class="proof-score">' + t().score + ' ' + score + ' / 3</p>' +
      '<div class="proof-actions">' +
      '<button id="retryBtn" class="proof-primary" type="button">' + t().retry + '</button>' +
      '<a href="../">' + t().backCamp + '</a>' +
      '</div>' +
      '</section>';
    $('retryBtn').addEventListener('click', function () {
      state.qIndex = 0;
      state.selected = null;
      state.answers = [];
      state.score = 0;
      state.finished = false;
      state.passed = false;
      render();
    });
  }

  function renderQuiz(boss) {
    const question = boss.questions[state.qIndex];
    const answered = state.selected !== null;
    const isCorrect = answered && state.selected === question.answer;
    const choices = question.choices.map((choice, index) => {
      let cls = 'proof-choice';
      if (answered && index === question.answer) cls += ' correct';
      if (answered && index === state.selected && !isCorrect) cls += ' wrong';
      return '<button class="' + cls + '" type="button" data-choice="' + index + '"' +
        (answered ? ' disabled' : '') + '>' +
        '<span class="proof-choice-letter">' + String.fromCharCode(65 + index) + '</span>' +
        '<span>' + esc(textOf(choice)) + '</span></button>';
    }).join('');

    const feedback = answered
      ? '<div class="proof-feedback ' + (isCorrect ? 'ok' : 'bad') + '">' +
        '<strong>' + (isCorrect ? t().correct : t().wrong) + '</strong>' +
        '<p>' + esc(textOf(question.why)) + '</p></div>'
      : '';

    $('proofApp').innerHTML =
      '<section class="proof-panel proof-quiz">' +
      '<div class="proof-quiz-head">' +
      '<div><p class="proof-kicker">' + esc(textOf(boss.tag)) + ' · ' + esc(boss.name) + '</p>' +
      '<h2>' + t().question + ' ' + (state.qIndex + 1) + ' ' + t().of + ' ' + boss.questions.length + '</h2></div>' +
      '<span class="proof-score">' + t().score + ' ' + state.score + ' / 3</span>' +
      '</div>' +
      '<div class="proof-dots">' + boss.questions.map((_, index) =>
        '<span class="' + (index < state.qIndex ? 'done' : (index === state.qIndex ? 'current' : '')) + '"></span>'
      ).join('') + '</div>' +
      '<p class="proof-question">' + esc(textOf(question.q)) + '</p>' +
      '<div class="proof-choices">' + choices + '</div>' +
      feedback +
      '<div class="proof-actions">' +
      (answered
        ? '<button id="nextBtn" class="proof-primary" type="button">' +
          (state.qIndex === boss.questions.length - 1 ? t().finish : t().next) + '</button>'
        : '') +
      '<a href="../">' + t().backCamp + '</a>' +
      '</div>' +
      '</section>';

    if (answered) {
      $('nextBtn').addEventListener('click', function () {
        if (state.qIndex < boss.questions.length - 1) {
          state.qIndex += 1;
          state.selected = null;
          render();
        } else {
          state.finished = true;
          state.passed = state.score === boss.questions.length;
          if (state.passed) {
            const proof = proofState();
            proof[boss.quest] = { passed: true, score: state.score, at: Date.now() };
            try { localStorage.setItem(PROOF_KEY, JSON.stringify(proof)); } catch (_) {}
            renderDone(boss, state.score);
          } else {
            renderFailed(boss, state.score);
          }
        }
      });
    }

    document.querySelectorAll('[data-choice]').forEach((button) => {
      button.addEventListener('click', function () {
        if (state.selected !== null) return;
        const index = Number(button.dataset.choice);
        state.selected = index;
        state.answers.push(index);
        if (index === question.answer) state.score += 1;
        render();
      });
    });
  }

  function render() {
    const boss = state.boss;
    if (!boss) {
      renderPicker();
      return;
    }
    if (state.finished) {
      if (state.passed) renderDone(boss, state.score);
      else renderFailed(boss, state.score);
      return;
    }
    if (isProofed(boss)) {
      renderDone(boss, 3);
      return;
    }
    if (!isCleared(boss)) {
      renderLocked(boss);
      return;
    }
    renderQuiz(boss);
  }

  const params = new URLSearchParams(location.search);
  state.boss = bossBySlug(params.get('boss'));

  $('themeBtn').addEventListener('click', function () {
    state.theme = state.theme === 'arcade' ? 'retro' : 'arcade';
    save(); applyChrome();
  });

  $('langBtn').addEventListener('click', function () {
    state.lang = state.lang === 'en' ? 'zh' : 'en';
    save(); applyChrome(); render();
  });

  applyChrome();
  render();
})();
