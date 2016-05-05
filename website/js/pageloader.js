// This file will load in the parts of the page


var siteUrl = "site-chunks/";

function getChunk(containerId, page) {
    $.get(siteUrl + page + ".html").then(function (data) {
        $(containerId).html(data);
    })
}

function getNav() {
    $.get(siteUrl + 'nav.html').then(function (data) {
        $('#nav').html(data);
    })
}

function getTeam() {
    $.get(siteUrl + 'team_content.html').then(function (data) {
        $('#team-content').html(data);
    })
}

function getProjectDetails() {
    $.get(siteUrl + 'project_details.html').then(function (data) {
        $('#project-content').html(data);
    })
}

function getFooter() {
    $.get(siteUrl + 'footer.html').then(function (data) {
        $('#footer').html(data);
    })
}

function getHeader() {
    $.get(siteUrl + 'header.html').then(function (data) {
        $('#content-header').html(data);
    })
}
