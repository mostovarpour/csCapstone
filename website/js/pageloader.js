// This file will load in the parts of the page
$(document).ready(function () {
    getNav();
    getHeader();
    getContent();
    getFooter();
});

function getNav() {
    $.get('site-chunks/nav.html').then(function (data) {
        $('#nav').html(data);
    })
}

function getContent() {
    $.get('site-chunks/content.html').then(function (data) {
        $('#main-content').html(data);
    })
}

function getFooter() {
    $.get('site-chunks/footer.html').then(function (data) {
        $('#footer').html(data);
    })
}

function getHeader() {
    $.get('site-chunks/header.html').then(function (data) {
        $('#content-header').html(data);
    })
}
