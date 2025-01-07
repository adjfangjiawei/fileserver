#!/usr/bin/env bash
set -euo pipefail

# 获取本地已加载的镜像，排除 repository 或 tag 为 <none> 的镜像
images=$(nerdctl images --format '{{.Repository}}:{{.Tag}}' | grep -v '<none>')

for image in $images; do
  repository=$(echo "$image" | cut -d':' -f1)
  echo "Processing repository: $repository"
  tag=$(echo "$image" | cut -d':' -f2)
  echo "Processing tag: $tag"

  # 再次检查，防止出现漏网的 <none> 镜像
  if [ "$repository" = "<none>" ] || [ "$tag" = "<none>" ]; then
    continue
  fi

  # 若仓库名里包含 "/", 替换为 "-"，方便作为文件名使用
  safe_repo=$(echo "$repository" | tr '/' '-')
  filename="${safe_repo}-${tag}.tar"

  echo "Saving $image -> $filename"
  nerdctl -n k8s.io save -o "$filename" "$image"
done