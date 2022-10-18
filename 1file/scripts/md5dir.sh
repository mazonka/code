#!/bin/bash
shopt -s globstar
md5sum "$1"/** > "${1}_list.md5"
md5sum "${1}_list.md5"