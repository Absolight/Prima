package Prima;

use strict;
require DynaLoader;
use vars qw($VERSION @ISA);
@ISA = qw(DynaLoader);

$VERSION = '0.01';
bootstrap Prima $VERSION;
$::application = undef;
require Prima::Const;
require Prima::Classes;

sub END
{
   &Prima::cleanup();
}

sub run
{
   die "Prima was not properly initialized\n" unless defined $::application;
   $::application-> go if $::application-> alive;
}

sub import
{
   my @modules = @_;
   for my $module (@modules) {
      $module = "Prima::$module" unless $module =~ /^Prima::/;
      eval "require $module;" if $module;
   }
}

1;
