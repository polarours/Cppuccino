// Keep <html lang> in step with the page the reader is actually viewing.
// MkDocs uses one theme language for the whole site, so Chinese pages need
// this correction; Material's instant navigation also needs it after a swap.
(function () {
  'use strict';

  function isChinese() {
    var path = window.location.pathname;
    // Browsers expose non-ASCII path segments percent-encoded. Decode first,
    // otherwise a page such as /docs/中文版/ never matches the Chinese test.
    try {
      path = decodeURIComponent(path);
    } catch (_) {}
    return path.indexOf('/zh/') !== -1 ||
      path.indexOf('/中文版/') !== -1 ||
      /\/zh$/.test(path);
  }

  function apply() {
    document.documentElement.lang = isChinese() ? 'zh-CN' : 'en';
  }

  apply();

  if (window.document$ && typeof window.document$.subscribe === 'function') {
    window.document$.subscribe(apply);
  } else {
    document.addEventListener('DOMContentLoaded', apply);
  }
})();
