
# index tree

## count distinct

### (online) 값의 배열이 주어질 때, [0, R) 구간에서 distinct 값의 개수 구하기

각 값의 처음 등장하는 위치가 중요하다. 처음 등장하는 위치에는 1을 적고, 나머진 0을 적는다. (index → count) sum tree를 만든다. [0, R)의 range sum을 계산한다.

### (offline) 값의 배열이 주어질 때, [L, R) 구간에서 distinct 값의 개수 구하기

위의 아이디어를 그대로 이용하는데, [L, R)을 L이 작은 것부터 순서대로 처리한다.
L을 늘릴 때, 그 위치에 1이 있었다면 그 값이 다음에 어디 등장하는지 알아내서,
원래 위치의 1을 지우고, 새로운 위치에 1을 더한다. [L, R)의 range sum을 계산한다.

### (online) 값의 배열이 주어질 때, [L, R) 구간에서 distinct 값의 개수 구하기

위의 오프라인을 사용하고, persistent tree를 만든다. 

### (online, single update) 값의 배열이 주어지고, 하나씩 변경이 가능할 때 [L, R) 구간에서 distinct 값의 개수 구하기

dynamic wavelet tree를 사용한다. 각 노드에 말단노드 개수를 기록한다. 변경은 제거와 추가를 통해 할 수 있다. O(log A log n), A는 최대값

## k-th number

### (offline) 값의 배열이 주어질 때, [0, R) 구간에서 k번째 수를 구한다.

(value → count) sum tree를 만든다. R를 하나씩 늘려가면서 해당 value의 count를 증가시킨다. 위에서부터 k를 빼 나가면서 원하는 값으로 찾아간다.

### (online) 값의 배열이 주어질 때, [L, R) 구간에서 k번째 수를 구한다.

위의 오프라인을 사용하고, persistent tree를 만든다. ([0, R) 트리) - ([0, L) 트리)에서 탐색한다.

또는 간단히 wavelet tree를 사용한다.

### (online, single update) 값의 배열이 주어질 때, [L, R) 구간에서 k번째 수를 구한다.

dynamic wavelet tree를 사용한다.


