 curl -X POST   http://192.168.0.2:7856/v1/files   -H "Content-Type: application/json"   -d '{
    "file_operations": [
      {
        "file_operation_type": "FILE_OPERATION_TYPE_UPLOAD_FILE",
        "file_name": "example.txt",
        "file_sample_md5": "abc123",
        "operation_status": "FILE_OPERATION_STATUS_IN_PROGRESS",
        "minio_key": "minio-path/example.txt",
        "upload_file_policy": {
          "upload_method": "UPLOAD_METHOD_FULL",
          "is_permanent": false,
          "file_server_type": "FILE_SERVER_TYPE_MINIO"
        },
        "upload_id": 123
      },
      {
        "file_operation_type": "FILE_OPERATION_TYPE_DOWNLOAD_FILE",
        "file_name": "anotherFile.txt",
        "file_sample_md5": "def456",
        "operation_status": "FILE_OPERATION_STATUS_UNSPECIFIED",
        "minio_key": "minio-path/anotherFile.txt",
        "download_file_policy": {
          "download_method": "DOWNLOAD_METHOD_SPLIT",
          "is_permanent": true,
          "file_server_type": "FILE_SERVER_TYPE_MINIO"
        },
        "upload_id": 456
      }
    ]
  }' -v