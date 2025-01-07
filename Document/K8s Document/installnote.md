# kubespray 安装

ansible-playbook -i inventory/testcluster/inventory.ini --user=root -b cluster.yml

# 创建 provisioner

kubectl apply -f https://raw.githubusercontent.com/rancher/local-path-provisioner/master/deploy/local-path-storage.yaml

# 设置 provisioner 创建的 storage class 的 cm

kubectl edit configmap local-path-config -n local-path-storage

# 修改这里的 path 为想要存储的路径

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

# 安装 helm

curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/ helm/main/scripts/get-helm-3  
chmod 700 get_helm.sh

# 加入 mysql 的 repo

helm repo add bitnami https://charts.bitnami.com/bitnami  
helm repo update

# 创建 mysql

helm install mysql bitnami/mysql \
--namespace mysql \
--create-namespace \
--set primary.persistence.storageClass=local-path \
--set primary.persistence.size=5Gi

# 获得 mysql 密码

kubectl -n mysql get secret my-mysql -o yaml

# 删除带 finalizer 的资源

NS=`kubectl get ns |grep Terminating | awk 'NR==1 {print $1}'` && kubectl get namespace "$NS" -o json   | tr -d "\n" | sed "s/\"finalizers\": \[[^]]\+\]/\"finalizers\": []/"   | kubectl replace --raw /api/v1/namespaces/$NS/finalize -f -

# 安装 neo4j

helm repo add neo4j https://helm.neo4j.com/
helm repo update

# 模板

```yaml
neo4j:
  name: volume-selector
  minimumClusterSize: 1
  acceptLicenseAgreement: "yes"
  edition: enterprise
volumes:
  data:
    mode: volumeClaimTemplate
    volumeClaimTemplate:
      storageClassName: local-path
      accessModes:
        - ReadWriteOnce
      resources:
        requests:
          storage: 10Gi
```

# 运行

```
helm install neo4j neo4j/neo4j -f neo4j-cluster.yaml -n neo4j
```

# 问题与解决方法

network": plugin type="calico" failed (add): error getting ClusterInformation: connection is unauthorized: Unauthorized  
这种问题可以通过重启 calico 解决

# 安装 clickhouse operator

```
curl -s https://raw.githubusercontent.com/Altinity/clickhouse-operator/master/deploy/operator-web-installer/clickhouse-operator-install.sh | OPERATOR_NAMESPACE=clickhouse bash
```

# 删除 iptables 防火墙规则

sudo iptables -F
sudo iptables -X

# 禁用 prometheus 网络策略

kubectl -n prometheus delete networkpolicy --all

# 给匿名用户授权

kubectl create clusterrolebinding test:anonymous --clusterrole=cluster-admin --user=system:anonymous
