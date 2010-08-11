#!/bin/bash
#
# wfelipe <wfelipe@gmail.com>
#

GITDIR="/srv/git/repositories"
BKPDIR="/export/bkp/git"

log()
{
	msg=$1

	echo `date` - $msg
}

do_backup_git()
{
	local repo_origin=$1
	local repo_dest=$2

	log "do_backup_git $repo_origin $repo_dest"
	if [ -d "$repo_dest" ]
	then
		pushd $repo_dest
		log "updating repository $repo_origin -> $repo_dest"
		if [ "`git remote | grep original-local | wc -l`" != 1 ]
		then
			log "adding remote mirror original-local"
			git remote add --mirror original-local file://$repo_origin
		fi
		git remote update
		popd
	else
		log "cloning repository $repo_origin -> $repo_dest"
		git clone --bare file://$repo_origin $repo_dest
	fi
}

do_dir()
{
	local origin=$1
	local dest=$2

	if [ -f "$origin/HEAD" ]
	then
		do_backup_git $origin $dest
	# in case the repository is not bare
	elif [ -d "$origin/.git" ]
		do_backup_git $origin $dest
	else
		for i in $origin/*
		do
			dir="`basename $i`"
			log "do_dir $origin/$dir $dest/$dir"
			do_dir "$origin/$dir" "$dest/$dir"
		done
	fi
}

log "Starting git backup"
log "origin: $GITDIR, dest: $BKPDIR"
do_dir $GITDIR $BKPDIR
log "Finished git backup"
