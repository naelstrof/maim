function test {
    "$@"
    local status=$?
    if [ $status -ne 0 ]; then
        echo "Error test \"$@\" failed with $status, should be 0!" >&2
        exit 1
    fi
}

function test_fail {
    "$@"
    local status=$?
    if [ $status -ne 1 ]; then
        echo "Error test \"$@\" failed with $status, should be 1!" >&2
        exit 1
    fi
}

echo "Starting unit tests..."
test ./maim
# Variable expansion tests.
test ./maim ~/test.png
test ./maim "~/test.png"

# Buffer overflow tests. Lots of characters depending on the username. With naelstrof it's over 2.5k which is over the default MAXPATHLENGTH of 1024.
test ./maim "~/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/../$USER/test.png"
test ./maim "/tmp/$(date +%s-%N).png" -g 1920x1080+0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000+-00000000000000000000000000000000000000000000000000000000000000000000000000000
test ./maim "/tmp/$(date +%s-%N).png" -x 0000000000000000000000000000000000000000000000000000 -y 0000000000000000000000000000000000000000000 -w 1920 -h 1080
test ./maim "/tmp/$(date +%s-%N).png" -s -c 1.00000000000000000000000000000000000000000000,1.00000000000000000000000000000000000000000000000,1.0000000000000000000000000000000000000000000000000,1.0000000000000000000000000000000000000000000000000
# Color RGB test (vs RGBA)
test ./maim "/tmp/$(date +%s-%N).png" -s -c 1,0,0

# Failure cases.
test_fail ./maim "/tmp/$(date +%s-%N).png" "~/whyamihere.png" 2>/dev/null
test_fail ./maim "/tmp/$(date +%s-%N).png" -x 0 -y 0 -w 100 2>/dev/null
test_fail ./maim "/tmp/$(date +%s-%N).png" -g thisisntageo 2>/dev/null
test_fail ./maim "~/test.this.is.not.a.real.extension" 2>/dev/null

rm "~/test.png"

echo "Unit tests finished without error! Check /tmp for any awkward/broken screenshots."
