param($p1, $p2)

if ($p1 -and $p2)
{
    $ndkpath = Get-Content "./ndkpath.txt"
    $child = Get-ChildItem -Path $ndkpath/toolchains/llvm/prebuilt -Name
    & $ndkpath/toolchains/llvm/prebuilt/$child/bin/llvm-addr2line -e ./build/debug/$p1 $p2
}
else
{
	echo give at least 1 argument
}
