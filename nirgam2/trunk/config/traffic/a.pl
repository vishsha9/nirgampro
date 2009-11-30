#! /usr/bin/perl

`rm tile-*`;

for($i=0;$i<64;$i++){
	`cp conf tile-$i`;
}
