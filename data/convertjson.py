import json
from datasets import load_dataset
import os

def create_json_from_s2orc(output_file="data.json", max_samples=1000, selected_columns=None):
    """
    Mengkonversi dataset S2ORC ke file JSON dengan kolom yang dipilih
    
    Args:
        output_file (str): Nama file output JSON
        max_samples (int): Maksimal jumlah sampel yang diambil
        selected_columns (list): Kolom yang ingin diambil
    """
    
    print("Loading dataset S2ORC...")
    try:
        # Load dataset
        ds = load_dataset("leminda-ai/s2orc_small")
        
        # Ambil split train (atau split yang tersedia)
        if 'train' in ds:
            data = ds['train']
        else:
            # Ambil split pertama yang tersedia
            split_name = list(ds.keys())[0]
            data = ds[split_name]
            print(f"Using split: {split_name}")
        
        print(f"Dataset loaded successfully! Total samples: {len(data)}")
        
        # Tampilkan kolom yang tersedia
        print("\nKolom yang tersedia:")
        for i, col in enumerate(data.column_names, 1):
            print(f"{i}. {col}")
        
        # Jika tidak ada kolom yang dipilih, gunakan kolom default yang ringan
        if selected_columns is None:
            # Pilih kolom yang paling penting dan tidak terlalu berat
            selected_columns = []
            available_cols = data.column_names
            
            # Prioritas kolom yang biasanya ada dan berguna
            priority_cols = ['title', 'abstract', 'authors', 'year', 'venue', 'paper_id', 'doi']
            
            for col in priority_cols:
                if col in available_cols:
                    selected_columns.append(col)
            
            # Jika tidak ada kolom prioritas, ambil 3-5 kolom pertama
            if not selected_columns:
                selected_columns = available_cols[:min(5, len(available_cols))]
        
        print(f"\nKolom yang dipilih: {selected_columns}")
        
        # Ambil sampel data
        sample_size = min(max_samples, len(data))
        print(f"Mengambil {sample_size} sampel dari {len(data)} total data...")
        
        # Konversi ke format yang bisa di-serialize ke JSON
        json_data = []
        
        for i in range(sample_size):
            sample = {}
            for col in selected_columns:
                if col in data.column_names:
                    value = data[i][col]
                    # Konversi ke format yang JSON-serializable
                    if isinstance(value, (list, dict, str, int, float, bool)) or value is None:
                        sample[col] = value
                    else:
                        sample[col] = str(value)
                        
            json_data.append(sample)
            
            # Progress indicator
            if (i + 1) % 100 == 0:
                print(f"Processed {i + 1}/{sample_size} samples...")
        
        # Simpan ke file JSON
        print(f"\nMenyimpan data ke {output_file}...")
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(json_data, f, indent=2, ensure_ascii=False)
        
        # Info file yang dihasilkan
        file_size = os.path.getsize(output_file) / (1024 * 1024)  # MB
        print(f"✅ Berhasil menyimpan {len(json_data)} samples ke {output_file}")
        print(f"📁 Ukuran file: {file_size:.2f} MB")
        
        # Tampilkan contoh data
        print(f"\n📋 Contoh data (sample pertama):")
        print(json.dumps(json_data[0], indent=2, ensure_ascii=False)[:500] + "...")
        
        return json_data
        
    except Exception as e:
        print(f"❌ Error: {e}")
        return None

def custom_column_selection():
    """
    Fungsi untuk memilih kolom secara interaktif
    """
    print("Loading dataset untuk melihat kolom yang tersedia...")
    ds = load_dataset("leminda-ai/s2orc_small")
    
    if 'train' in ds:
        data = ds['train']
    else:
        split_name = list(ds.keys())[0]
        data = ds[split_name]
    
    print("\nKolom yang tersedia:")
    cols = data.column_names
    for i, col in enumerate(cols, 1):
        print(f"{i}. {col}")
    
    print("\nMasukkan nomor kolom yang ingin diambil (pisahkan dengan koma):")
    print("Contoh: 1,2,3 atau tekan Enter untuk kolom default")
    
    selection = input("Pilihan Anda: ").strip()
    
    if not selection:
        return None  # Gunakan default
    
    try:
        indices = [int(x.strip()) - 1 for x in selection.split(',')]
        selected_cols = [cols[i] for i in indices if 0 <= i < len(cols)]
        return selected_cols
    except:
        print("Format input tidak valid, menggunakan kolom default...")
        return None

if __name__ == "__main__":
    print("🚀 S2ORC Dataset to JSON Converter")
    print("=" * 50)
    
    # Opsi 1: Gunakan setting default (cepat)
    print("\n1. Gunakan setting default (1000 samples, kolom penting)")
    print("2. Pilih kolom secara manual")
    print("3. Custom setting")
    
    choice = input("\nPilihan Anda (1/2/3): ").strip()
    
    if choice == "2":
        # Pilih kolom manual
        selected_cols = custom_column_selection()
        create_json_from_s2orc(selected_columns=selected_cols)
        
    elif choice == "3":
        # Custom setting
        try:
            max_samples = int(input("Berapa sampel yang ingin diambil? (default: 1000): ") or "1000")
            output_file = input("Nama file output (default: data.json): ").strip() or "data.json"
            
            print("\nPilih kolom:")
            selected_cols = custom_column_selection()
            
            create_json_from_s2orc(
                output_file=output_file,
                max_samples=max_samples,
                selected_columns=selected_cols
            )
        except ValueError:
            print("Input tidak valid, menggunakan setting default...")
            create_json_from_s2orc()
    
    else:
        # Default
        create_json_from_s2orc()
    
    print("\n✨ Selesai!")