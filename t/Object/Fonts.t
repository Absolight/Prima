use strict;
use warnings;

use Test::More;
use Prima::Test;

plan tests => 9;

my $x = Prima::DeviceBitmap-> create( monochrome => 1, width => 8, height => 8);

for ( qw( height size direction)) {
       my $fx = $x-> font-> $_();
       $x-> font( $_ => $x-> font-> $_() * 3 + 12);
       my $fx2 = $x-> font-> $_();
       SKIP: {
           if ( $fx2 == $fx) {
               skip "$_", 1;
               next;
           }
           $x-> font( $_ => $fx);
           is( $fx, $x-> font-> $_(), "$_");
       };
}

# width is a bit special, needs height or size
for (qw( height size)) {
	my $fh = $x-> font-> $_();
	my $fw = $x-> font-> width;
	$x-> font( $_ => $fh, width => $fw * 3 + 12);
	my $fw2 = $x-> font-> width;
	SKIP: {
	    if ( $fw2 == $fw) {
	        skip "width", 1;
	        next;
	    }
	    $x-> font( $_ => $fh, width => $fw );
	    is( $fw, $x-> font-> width, "width by $_");
	};
}

my $fx = $x-> font-> pitch;
my $newfx = ( $fx == fp::Fixed) ? fp::Variable : fp::Fixed;
$x-> font( pitch => $newfx);
my $fx2 = $x-> font-> pitch;
$x-> font( pitch => $fx);
is( $x-> font-> pitch, $fx, "pitch");
is( $fx2, $newfx, "pitch");

$fx = $x-> font-> style;
$newfx = ~$fx;
$x-> font( style => $newfx);
$fx2 = $x-> font-> style;
SKIP : {
    if ( $fx2 == $fx) {
        skip "style", 1;
    }
    $x-> font( style => $fx);
    is( $fx, $x-> font-> style, "style");
};

$x-> font-> height( 16);
my $w = $x-> width;
cmp_ok( scalar @{$x-> text_wrap( "Ein zwei drei fir funf sechs seben acht neun zehn", $w * 5)}, '>', 4, "text wrap");

$x-> destroy;
