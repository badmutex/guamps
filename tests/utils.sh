set -x

export GMX_MAXBACKUP=-1

IN_TPR=data/topol.tpr
OUT_TPR=data/out.tpr

fail() {
    echo ERROR
    cat out.log
    exit 1
}

gget() {
    tpr=$1
    sel=$2
    out=$3
    test -z $out || out="-o $out"
    guamps_get -f $tpr -s $sel $out
}

gset() {
    itpr=$1
    otpr=$2
    sel=$3
    inp=$4
    test -z $inp || inp="-i $inp"
    guamps_set -f $itpr -o $otpr -s $sel $inp
}

check_scalar() {
    ref=$(echo $1 | bc -l)
    val=$(echo $2 | bc -l)

    if [ $(echo "$1 == $2" | bc) -eq 1 ]; then
	echo "OK"
	return 0;
    else:
	echo "ERROR: expected $ref but got $val"
	exit 1;
    fi
}

check_files() {
    ref=$1
    val=$2
    diff -y $ref $val >diff
    if [[ ! $? -eq 0 ]]; then
	echo "ERROR:"
	cat diff
	return 1
    else
	echo "OK"
	return 0
    fi
}

tpr_get_scalar() {
    tpr=$1
    name=$2
    gmxdump -s $tpr 2>&1 | grep $name | awk '{print $3}'
}

tpr_get_vector() {
    tpr=$1
    name=$2

    case $name in
	positions)
	    editconf -f $tpr -o x.pdb >/dev/null 2>&1
	    grep ATOM x.pdb | awk '{print $6,$7,$8}'
	    ;;
	velocities)
	    editconf -f $tpr -o v.gro >/dev/null 2>&1
	    tail -n +3 v.gro | awk '{print $7,$8,$9}' | head -551
	    ;;
	*)
	    echo "ERROR: I don't know how to get $name from $tpr"

	    ;;
    esac # | tr ' ' '\n'
}

random_int() {
    max=$1
    echo "$RANDOM % $max" | bc
}
    

random_float() {
    echo "scale=5; $RANDOM*1.0/32767" | bc
}

random_coord() {
    i=$(random_int 99)
    f=$(random_float)
    f=${f:0:4}
    echo $i$f
}

random_vector() {
    natoms=$1
    lines=$(echo "$natoms * 3" | bc)
    echo "ncells: 1"
    echo "ncoords: $natoms"
    echo "ndims: 3"
    echo
    for i in $(seq 1 $lines); do
	echo $(random_coord)
    done
}

test-scalar() {
    sel=$1
    grepname=$2
    test -z $grepname && grepname=$sel

    echo -n "    get..."
    gget $IN_TPR $sel out.gps >/dev/null 2>&1
    ref=$(tpr_get_scalar $IN_TPR $grepname)
    check_scalar \
	$ref \
	$(cat out.gps)

    echo -n "    set..."
    val=42
    echo $val >out.gps
    gset $IN_TPR $OUT_TPR $sel out.gps >/dev/null 2>&1
    ref=$(tpr_get_scalar $OUT_TPR $grepname)
    check_scalar \
	$val \
	$ref
}

test-vector() {
    sel=$1

    echo -n "    get..."
    gget $IN_TPR $sel out.gps >out.log 2>&1
    test $? -eq 0 && echo OK || fail

    echo -n "    set..."
    gset $IN_TPR $OUT_TPR $sel out.gps >out.log 2>&1
    test $? -eq 0 && echo OK || fail
}
