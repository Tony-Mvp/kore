if ! pgrep -u "$USER" ssh-agent > /dev/null; then
   eval "$(ssh-agent -s)"
fi

ssh-add ~/.ssh/id_ed25519 2>/dev/null
