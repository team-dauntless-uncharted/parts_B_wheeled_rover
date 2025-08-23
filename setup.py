#!/usr/bin/python3

import subprocess
import os

def update_submodules_in_directory(submodule_dir):
    """
    指定されたディレクトリ内のサブモジュールを初期化・更新する関数。
    """
    if not os.path.isdir(submodule_dir):
        print(f"エラー: 指定されたディレクトリ '{submodule_dir}' が見つかりません。")
        return

    print(f"ディレクトリ '{submodule_dir}' 内のサブモジュールを初期化・更新中...")
    try:
        # 指定されたディレクトリに移動
        os.chdir(submodule_dir)
        
        # サブモジュールを初期化・更新
        subprocess.run(["git", "submodule", "update", "--init"], check=True)
        
        print("サブモジュールの更新が完了しました。")
        
    except FileNotFoundError:
        print("エラー: 'git' コマンドが見つかりません。Gitが正しくインストールされているか確認してください。")
    except subprocess.CalledProcessError as e:
        print(f"Gitコマンドの実行中にエラーが発生しました: {e}")
    except Exception as e:
        print(f"予期せぬエラーが発生しました: {e}")

if __name__ == "__main__":
    # サブモジュールがあるディレクトリを指定
    # このスクリプトは、ルートディレクトリで実行されることを想定しています。
    submodule_directory = "libraries"
    
    update_submodules_in_directory(submodule_directory)