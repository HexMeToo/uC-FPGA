library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity Licznik_W is
	port(	clk: in std_logic;
			QB: buffer std_logic_vector(1 downto 0);
			Q: buffer std_logic_vector(3 downto 0) );
end Licznik_W;

architecture Behavioral of Licznik_W is
begin
	process(clk)
	begin
		if clk'event and clk = '0' then
			case Q is
				when "1110" =>   Q <= "1101"; QB <= "01";
				when "1101" =>   Q <= "1011"; QB <= "10";
				when "1011" =>   Q <= "0111"; QB <= "11";
				when "0111" =>   Q <= "1110"; QB <= "00";
				when others =>   Q <= "1110"; QB <= "00";
			end case;
		end if;
	end process;
end Behavioral;