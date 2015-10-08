// This file will load in the parts of the page
$(document).ready(function () {
    getNav();
    getHeader();
    getTeam();
    getProjectDetails();
    getFooter();
});

function getNav() {
    $.get('site-chunks/nav.html').then(function (data) {
        $('#nav').html(data);
    })
}

function getTeam() {
    $.get('site-chunks/team_info.html').then(function (data) {
        $('#team-content').html(data);
    })
}

function getProjectDetails() {
    $.get('site-chunks/project_details.html').then(function (data) {
        $('#project-content').html(data);
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
