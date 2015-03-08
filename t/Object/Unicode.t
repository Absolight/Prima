use Test::More tests => 4;

use strict;
use warnings;

use Prima::Test;
use Prima::Application;

if( $Prima::Test::noX11 ) {
    plan skip_all => "Skipping all because noX11";
}

unless ( $] >= 5.006 && 
	$::application-> get_system_value( sv::CanUTF8_Output)
) {
	print "1..1 support\n";
	skip;
	return 1;
}

pass("support");

my $utf8_line;
eval '$utf8_line="line\\x{2028}line"';
my @r = @{$::application-> text_wrap( $utf8_line, 1000, tw::NewLineBreak)};
cmp_ok( scalar @r, '==', 2, "wrap utf8 text");
ok( @r, "wrap utf8 text"  );
is( $r[0], $r[1], "wrap utf8 text" );

done_testing();
