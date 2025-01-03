# 创建provisioner
kubectl apply -f https://raw.githubusercontent.com/rancher/local-path-provisioner/master/deploy/local-path-storage.yaml
# 设置provisioner创建的storage class的cm
kubectl edit configmap local-path-config -n local-path-storage
# 修改这里的path为想要存储的路径
```yaml
- apiVersion: v1
  data:
    config.json: |-
      {
              "nodePathMap":[
              {
                      "node":"DEFAULT_PATH_FOR_NON_LISTED_NODES",
                      "paths":["/installdev/storage"]
              }
              ]
      }
```
# 安装helm
curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/  helm/main/scripts/get-helm-3  
chmod 700 get_helm.sh

# 加入mysql的repo
helm repo add bitnami https://charts.bitnami.com/bitnami  
helm repo update

# 创建mysql
helm install my-mysql bitnami/mysql  
--namespace mysql  
--set primary.persistence.storageClass=local-path  
--set primary.persistence.size=5Gi

# 获得mysql密码
kubectl -n mysql get secret my-mysql -o yaml

# 删除带finalizer的资源
NS=`kubectl get ns |grep Terminating | awk 'NR==1 {print $1}'` && kubectl get namespace "$NS" -o json   | tr -d "\n" | sed "s/\"finalizers\": \[[^]]\+\]/\"finalizers\": []/"   | kubectl replace --raw /api/v1/namespaces/$NS/finalize -f -