Utilities for organising large quantity of large files. 
Stbcmp can compare and copy directories. It also can 
find same files (useful when you add files to an archive 
without knowing if same or similar files are already present).

NB: stbcmp cannot create directories (C++ standard) so it cannot
copy the directory structure; to recreate directory structure
use copytree.sh script
