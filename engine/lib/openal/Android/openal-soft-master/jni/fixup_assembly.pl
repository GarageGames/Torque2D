while (<>) {
  s/chkstk/_chkstk/;
  s/rrx(.*)/mov\1, rrx/;
  print;
}