#!/usr/bin/perl

print "Uploading TAP - Will check for 512 multiple bug\n";

$tmp = `ls -l`;

@files = split(/\n/, $tmp );
foreach(@files){
	if( $_ =~ /\.tap/ ){
		@tmp2 = split(/ /, $_ );
		if( (@tmp2[5]%512) != 0 ){
			system('puppy -c put "multiviewepg.tap" "ProgramFiles\\Auto Start\\multiviewepg.tap"');
#			system('puppy -c put "multiviewepg.tap" "ProgramFiles\\multiviewepg.tap"');
			print "TAP uploaded\n";
#			system('puppy -c dir "ProgramFiles\\Auto Start"');
		} else {
			print "TAP is a multiple of 512. Transfer aborted\n";
		}
	}
}

#puppy -c put "multiviewepg.tap" "ProgramFiles\\multiviewepg.tap"
#puppy -c put "multiviewepg.tap" "ProgramFiles\\Auto Start\\multiviewepg.tap"
